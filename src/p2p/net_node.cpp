// Copyright (c) 2018, Ombre Cryptocurrency Project
// Copyright (c) 2018, Ryo Currency Project
// Portions copyright (c) 2014-2018, The Monero Project
//
// Portions of this file are available under BSD-3 license. Please see ORIGINAL-LICENSE for details
// All rights reserved.
//
// Ombre changes to this code are in public domain. Please note, other licences may apply to the file.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
// THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
// THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Parts of this file are originally copyright (c) 2012-2013 The Cryptonote developers

#include "net_node.h"
#include "common/command_line.h"

namespace nodetool
{
const command_line::arg_descriptor<std::string> arg_p2p_bind_ip = {"p2p-bind-ip", "Interface for p2p network protocol", "0.0.0.0"};
const command_line::arg_descriptor<std::string, false, true, 2> arg_p2p_bind_port = {
	"p2p-bind-port", "Port for p2p network protocol", std::to_string(cryptonote::config<cryptonote::MAINNET>::P2P_DEFAULT_PORT), {{&cryptonote::arg_testnet_on, &cryptonote::arg_stagenet_on}}, [](std::array<bool, 2> testnet_stagenet, bool defaulted, std::string val) -> std::string {
          if (testnet_stagenet[0] && defaulted)
            return std::to_string(cryptonote::config<cryptonote::TESTNET>::P2P_DEFAULT_PORT);
          else if (testnet_stagenet[1] && defaulted)
            return std::to_string(cryptonote::config<cryptonote::STAGENET>::P2P_DEFAULT_PORT);
          return val; }};
const command_line::arg_descriptor<uint32_t> arg_p2p_external_port = {"p2p-external-port", "External port for p2p network protocol (if port forwarding used with NAT)", 0};
const command_line::arg_descriptor<bool> arg_p2p_allow_local_ip = {"allow-local-ip", "Allow local ip add to peer list, mostly in debug purposes"};
const command_line::arg_descriptor<std::vector<std::string>> arg_p2p_add_peer = {"add-peer", "Manually add peer to local peerlist"};
const command_line::arg_descriptor<std::vector<std::string>> arg_p2p_add_priority_node = {"add-priority-node", "Specify list of peers to connect to and attempt to keep the connection open"};
const command_line::arg_descriptor<std::vector<std::string>> arg_p2p_add_exclusive_node = {"add-exclusive-node", "Specify list of peers to connect to only."
																												 " If this option is given the options add-priority-node and seed-node are ignored"};
const command_line::arg_descriptor<std::vector<std::string>> arg_p2p_seed_node = {"seed-node", "Connect to a node to retrieve peer addresses, and disconnect"};
const command_line::arg_descriptor<bool> arg_p2p_hide_my_port = {"hide-my-port", "Do not announce yourself as peerlist candidate", false, true};

const command_line::arg_descriptor<bool> arg_no_igd = {"no-igd", "Disable UPnP port mapping"};
const command_line::arg_descriptor<int64_t> arg_out_peers = {"out-peers", "set max number of out peers", -1};
const command_line::arg_descriptor<int64_t> arg_in_peers = {"in-peers", "set max number of in peers", -1};
const command_line::arg_descriptor<int> arg_tos_flag = {"tos-flag", "set TOS flag", -1};

const command_line::arg_descriptor<int64_t> arg_limit_rate_up = {"limit-rate-up", "set limit-rate-up [kB/s]", -1};
const command_line::arg_descriptor<int64_t> arg_limit_rate_down = {"limit-rate-down", "set limit-rate-down [kB/s]", -1};
const command_line::arg_descriptor<int64_t> arg_limit_rate = {"limit-rate", "set limit-rate [kB/s]", -1};
}
