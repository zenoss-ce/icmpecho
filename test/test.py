##############################################################################
# 
# Copyright (C) Zenoss, Inc. 2011, all rights reserved.
# 
# This content is made available according to terms specified in
# License.zenoss under the directory where your Zenoss product is installed.
# 
##############################################################################


import unittest
import sys
import logging
from icmpecho.constants import ICMPv4_TYPE
from icmpecho.network import EchoEncoder, _compute_checksum

_LOG = logging.getLogger("icmpecho")

class TestEcho4(unittest.TestCase):

  def setUp(self):
    _LOG.debug('')

  def tearDown(self):
    pass

  def test_encode_echo(self):
    identifier = 0x42
    sequence = 0x0001
    data_size = 56
    echo4 = EchoEncoder(ICMPv4_TYPE.ECHO_REQUEST, identifier, sequence, data_size)
    echo4_packet = echo4.encode_packet()
    checksum = _compute_checksum(echo4._gen_bytes())
    # the checksum of a packet that already has the checksum set should be 0
    self.assertEqual(checksum, 0)

if __name__ == '__main__':
  if '-v' in sys.argv or '--verbose' in sys.argv:
    logging.basicConfig(level=logging.DEBUG)
  else:
    logging.basicConfig()
  unittest.main()
