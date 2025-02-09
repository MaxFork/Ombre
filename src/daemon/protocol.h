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

#pragma once

namespace daemonize
{

class t_protocol final
{
  private:
	typedef cryptonote::t_cryptonote_protocol_handler<cryptonote::core> t_protocol_raw;
	typedef nodetool::node_server<t_protocol_raw> t_node_server;

	t_protocol_raw m_protocol;

  public:
	t_protocol(
		boost::program_options::variables_map const &vm, t_core &core, bool offline = false)
		: m_protocol{core.get(), nullptr, offline}
	{
		MGINFO("Initializing cryptonote protocol...");
		if(!m_protocol.init(vm))
		{
			throw std::runtime_error("Failed to initialize cryptonote protocol.");
		}
		MGINFO("Cryptonote protocol initialized OK");
	}

	t_protocol_raw &get()
	{
		return m_protocol;
	}

	void set_p2p_endpoint(
		t_node_server &server)
	{
		m_protocol.set_p2p_endpoint(&server);
	}

	~t_protocol()
	{
		MGINFO("Stopping cryptonote protocol...");
		try
		{
			m_protocol.deinit();
			m_protocol.set_p2p_endpoint(nullptr);
			MGINFO("Cryptonote protocol stopped successfully");
		}
		catch(...)
		{
			LOG_ERROR("Failed to stop cryptonote protocol!");
		}
	}
};
}
