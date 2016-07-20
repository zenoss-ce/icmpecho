##############################################################################
# 
# Copyright (C) Zenoss, Inc. 2011, all rights reserved.
# 
# This content is made available according to terms specified in
# License.zenoss under the directory where your Zenoss product is installed.
# 
##############################################################################


import sys
import os
import logging
import socket
import signal
from optparse import OptionParser
from zope import component
from twisted.internet import reactor
from icmpecho.network import Network
from icmpecho.schedule import Scheduler
from icmpecho.hostAndPing import Ping, Host
from icmpecho.stdout import Stdout

DEFAULT_INTERVAL = 1.0

def parse_args():
  usage = "Usage: %prog [OPTIONS] -f filename | host [host [host ...]]"
  parser = OptionParser(usage=usage)
  parser.set_defaults(verbose=False,
                      family=socket.AF_UNSPEC,
                      data_size=56)
  parser.add_option("-v", "--verbose", action="store_true", help="log debug messages")
  parser.add_option("-4", action="store_const", const=socket.AF_INET,
                    dest="family", help="force the use of IPv4")
  parser.add_option("-6", action="store_const", const=socket.AF_INET6,
                    dest="family", help="force the use of IPv6")
  parser.add_option("-c", "--count", type="int", help="number of ICMP packets to send")
  parser.add_option("-i", "--interval", type="float", help="seconds to wait between sending ICMP packets")
  parser.add_option("-T", "--timeout", type="float", help="seconds to wait before considering dropped")
  parser.add_option("-t", "--ttl", type="int", help="time to live for each ICMP packet")
  parser.add_option("-Q", "--qos", type="int", help="Quality of Service (QoS) of outgoing packets")
  parser.add_option("-f", "--filename", help="filename to read hosts from")
  parser.add_option("-s", "--size", dest="data_size", type="int", help="size in bytes of the data field")
  options, args = parser.parse_args()
  if options.timeout is None:
    if options.interval is None:
      options.interval = DEFAULT_INTERVAL
    options.timeout = options.interval
  elif options.interval is None:
    options.interval = options.timeout
  return parser, options, args

def gen_hostnames(args, filename):
  for hostname in args:
    yield hostname
  if filename is not None:
    file_ = open(filename)
    for hostname in file_:
      yield hostname
    file_.close()

def gen_hosts(options, args):
  identifier = os.getpid()
  for hostname in gen_hostnames(args, options.filename):
    family, sockaddr = get_address_information(hostname, options.family)
    yield Host(hostname, family, sockaddr, identifier, options.data_size, options.ttl, options.qos)

def get_address_information(host, family=socket.AF_UNSPEC):
  for family_, socktype, proto, canonname, sockaddr in socket.getaddrinfo(host, None, family):
    if family_ in (socket.AF_INET, socket.AF_INET6):
      return family_, sockaddr
  raise StandardError("Could not resolve host '%s', for family " % (host, family))

def stop_reactor():
  if reactor.running:
      reactor.stop()

def main():
  parser, options, args = parse_args()
  if not args:
    parser.print_help()
    return 1
  if options.verbose:
    logging.basicConfig(level=logging.DEBUG)
  else:
    logging.basicConfig()
  component.provideAdapter(Stdout)
  host_list = list(gen_hosts(options, args))
  hosts = {}
  for host in host_list:
    hosts[host.address] = host
  # to make this program secure the sockets are injected into the main module by pyraw
  network = Network(IPV4_SOCKET, IPV6_SOCKET)
  ping = Ping(hosts, network)
  scheduler = Scheduler(ping, options.count, options.interval, options.timeout, stop_reactor)
  signal.signal(signal.SIGINT, scheduler.on_sigint)
  reactor.callWhenRunning(scheduler.start)
  try:
    reactor.run()
  except StandardError, e:
    stop_reactor()
    print >>sys.stderr, e
    return 1
  for host in host_list:
    host.output.write_statistics()
  return 0

if __name__ == '__main__':
  sys.exit(main())
