/*****************************************************************************
 * 
 * Copyright (C) Zenoss, Inc. 2011, all rights reserved.
 * 
 * This content is made available according to terms specified in
 * License.zenoss under the directory where your Zenoss product is installed.
 * 
 ****************************************************************************/


#include <stdio.h>
#include <arpa/inet.h>

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#define IPv4_PROTOCOL_ICMP 0x01

#define ICMPv4_TYPE_ECHO_REQUEST 0x08
#define ICMPv4_TYPE_ECHO_REPLY 0x00
#define ICMPv4_TYPE_DESTINATION_UNREACHABLE 0x03
#define ICMPv4_TYPE_TIME_EXCEEDED 0x0b

#define ICMPv6_TYPE_ECHO_REQUEST 0x80
#define ICMPv6_TYPE_ECHO_REPLY 0x81
#define ICMPv6_TYPE_DESTINATION_UNREACHABLE 0x01
#define ICMPv6_TYPE_TIME_EXCEEDED 0x03

typedef struct ipv4_header {
  unsigned int ihl:4;
  unsigned int version:4;
  u_int8_t qos;
  u_int16_t tot_len;
  u_int16_t id;
  u_int16_t frag_off;
  u_int8_t ttl;
  u_int8_t protocol;
  u_int16_t check;
  u_int32_t source_address;
  u_int32_t destination_address;
} ipv4_header_t;

typedef struct icmp_header
{
  u_int8_t type;
  u_int8_t code;
  u_int16_t checksum;
  u_int16_t identifier;
  u_int16_t sequence;
} icmp_header_t;

typedef struct embedded_info {
  char address[INET_ADDRSTRLEN];
  u_int8_t type;
  u_int16_t identifier;
  u_int16_t sequence;
} embedded_info_t;

typedef struct received_info {
  u_int8_t ttl;
  u_int8_t qos;
  char address[INET_ADDRSTRLEN];
  u_int8_t type;
  u_int16_t identifier;
  u_int16_t sequence;
  size_t data_size;
} received_info_t;

static uint16_t calculate_checksum(const char *icmp_packet, size_t icmp_packet_size) {
  uint16_t *ptr;
  size_t remaining = icmp_packet_size;
  uint32_t sum = 0;
  for (ptr = (uint16_t *) icmp_packet; remaining>1; ptr++, remaining-=2) {
    sum += *ptr;
  }
  if (remaining == 1) {
    uint16_t odd_byte;
    *(char *) &odd_byte = *(char *) ptr;
    sum += odd_byte;
  }
  sum = (sum >> 16) + (sum & 0xFFFF);
  sum += sum >> 16;
  return ~sum;
}

/*
 * sanity checks used by both decode4 and decode4_embedded
 */
static int decode4_shared(const char *ip_packet, size_t ip_packet_size, size_t *ip_header_size, size_t *icmp_packet_size) {
  if (ip_packet_size < sizeof(ipv4_header_t)) {
    PyErr_SetString(PyExc_RuntimeError, "Packet is too small to fit IP header.");
    return -1;
  }
  ipv4_header_t *ip_header = (ipv4_header_t *) ip_packet;
  *ip_header_size = ip_header->ihl * 4;
  if (ip_packet_size < *ip_header_size) {
    PyErr_SetString(PyExc_RuntimeError, "Packet size less than IP header length.");
    return -1;
  }
  if (ip_header->protocol != IPv4_PROTOCOL_ICMP) {
    PyErr_SetString(PyExc_RuntimeError, "IP protocol is not ICMP.");
    return -1;
  }
  *icmp_packet_size = ip_packet_size - *ip_header_size;
  if (*icmp_packet_size < sizeof(icmp_header_t)) {
    PyErr_SetString(PyExc_RuntimeError, "IP payload is too small to fit ICMP header.");
    return -1;
  }
  return 0;
}

/*
 * time-exceeded and host-unreachable messages embed the IP header and first 8 
 * bytes of the original datagram's data
 */
static int decode4_embedded(const char *icmp_packet, embedded_info_t *info) {
  const char *ip_packet = icmp_packet + sizeof(icmp_header_t);
  size_t ip_packet_size = sizeof(ipv4_header_t) + sizeof(icmp_header_t); 
  size_t ip_header_size;
  size_t icmp_packet_size;
  int status = decode4_shared(ip_packet, ip_packet_size, &ip_header_size, &icmp_packet_size);
  if (status) {
    return status;
  }
  ipv4_header_t *ip_header = (ipv4_header_t *) ip_packet;
  icmp_header_t *icmp_header = (icmp_header_t *) (ip_packet + ip_header_size);
  if (icmp_header->type != ICMPv4_TYPE_ECHO_REQUEST) {
    PyErr_SetString(PyExc_RuntimeError, "Embedded datagram has wrong ICMP type.");
    return -1;
  }
  info->type = icmp_header->type;
  inet_ntop(AF_INET, &ip_header->destination_address, info->address, INET_ADDRSTRLEN);
  info->identifier = ntohs(icmp_header->identifier);
  info->sequence = ntohs(icmp_header->sequence);
  return 0;
}

static int decode4(const char *ip_packet, size_t ip_packet_size, received_info_t *info) {
  size_t ip_header_size;
  size_t icmp_packet_size;
  int status = decode4_shared(ip_packet, ip_packet_size, &ip_header_size, &icmp_packet_size);
  if (status) {
    return status;
  }
  ipv4_header_t *ip_header = (ipv4_header_t *) ip_packet;
  const char *icmp_packet = ip_packet + ip_header_size;
  icmp_header_t *icmp_header = (icmp_header_t *) icmp_packet;
  info->ttl = ip_header->ttl;
  info->qos = ip_header->qos;
  info->type = icmp_header->type;
  switch (info->type) { 
    case ICMPv4_TYPE_ECHO_REPLY:
      if (calculate_checksum(icmp_packet,  icmp_packet_size) != 0) {
        PyErr_SetString(PyExc_RuntimeError, "Checksum error.");
        return -1;
      }
      inet_ntop(AF_INET, &ip_header->source_address, info->address, INET_ADDRSTRLEN);
      info->identifier = ntohs(icmp_header->identifier);
      info->sequence = ntohs(icmp_header->sequence);
      info->data_size = icmp_packet_size - sizeof(icmp_header_t);
      break;
    case ICMPv4_TYPE_DESTINATION_UNREACHABLE:
    case ICMPv4_TYPE_TIME_EXCEEDED:
      {
        embedded_info_t embedded_info;
        status = decode4_embedded(icmp_packet, &embedded_info);
        if (status) {
          return status;
        }
        strcpy(info->address, embedded_info.address);
        info->identifier = embedded_info.identifier;
        info->sequence = embedded_info.sequence;
      }
      break;
    default: {
      PyErr_SetString(PyExc_RuntimeError, "Unexpected ICMP type.");
      return -1;
    }
  }

  // printf("ttl=%u (0x%x) ", info->ttl, info->ttl);
  // printf("qos=%u (0x%x) ", info->qos, info->qos);
  // printf("address=%s ", info->address);
  // printf("type=%u (0x%x) ", info->type, info->type);
  // printf("code=%u (0x%x) ", icmp_header->code, icmp_header->code);
  // u_int16_t checksum = ntohs(icmp_header->checksum);
  // printf("checksum=%u (0x%x) ", checksum, checksum);
  // printf("identifier=%u (0x%x) ", info->identifier, info->identifier);
  // printf("sequence=%u (0x%x) ", info->sequence, info->sequence);
  // printf("data_size=%zu\n", info->data_size);

  return 0;
}

static int decode6(const char *icmp_packet, size_t icmp_packet_size, received_info_t *info) {
  if (icmp_packet_size < sizeof(icmp_header_t)) {
    PyErr_SetString(PyExc_RuntimeError, "IP payload is too small to fit ICMP header.");
    return -1;
  }
  icmp_header_t *icmp_header = (icmp_header_t *) icmp_packet;
  info->type = icmp_header->type;
  switch (info->type) {
    case ICMPv6_TYPE_ECHO_REPLY:
      info->identifier = ntohs(icmp_header->identifier);
      info->sequence = ntohs(icmp_header->sequence);
      info->data_size = icmp_packet_size - sizeof(icmp_header_t);;
      break;
    default:
      PyErr_SetString(PyExc_RuntimeError, "Unexpected ICMP type.");
      return -1;
  }
  return 0;
}

static void set_item_from_long(PyObject *dict, char *key, long value) {
  PyObject *v = PyInt_FromLong(value);
  PyDict_SetItemString(dict, key, v);
  Py_DECREF(v);
}

static PyObject *decode(PyObject *self, PyObject *args) {
  int family;
  const char *ip_packet;
  size_t ip_packet_size;
  if (!PyArg_ParseTuple(args, "is#", &family, &ip_packet, &ip_packet_size)) {
    return NULL;
  }
  received_info_t info;
  int status;
  switch (family) {
    case AF_INET:
      status = decode4(ip_packet, ip_packet_size, &info);
      break;
    case AF_INET6:
      status = decode6(ip_packet, ip_packet_size, &info);
      break;
    default:
      PyErr_SetString(PyExc_RuntimeError, "Address family not supported.");
      return NULL;
  }
  if (status) {
    return NULL;
  }
  PyObject *dict = PyDict_New();
  set_item_from_long(dict, "ttl", info.ttl);
  set_item_from_long(dict, "qos", info.qos);

  PyObject *addr_value = PyString_FromString(info.address);
  PyDict_SetItemString(dict, "address", addr_value);
  Py_DECREF(addr_value);

  set_item_from_long(dict, "type", info.type);
  set_item_from_long(dict, "identifier", info.identifier);
  set_item_from_long(dict, "sequence", info.sequence);
  set_item_from_long(dict, "data_size", info.data_size);
  return dict;
}

PyMODINIT_FUNC init_network(void) {
  static PyMethodDef methods[] = {{"decode", decode, METH_VARARGS, "Decode packet."},
                                  {NULL, NULL, 0, NULL},
                                  };
  Py_InitModule("_network", methods);
}
