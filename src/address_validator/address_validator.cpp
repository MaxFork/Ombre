// Copyright (c) 2018, Ombre Cryptocurrency Project
// Copyright (c) 2018, Ryo Currency Project
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

#include "common/command_line.h"
#include "address_validator/address_validator.h"
#include "string_tools.h"
#include "cryptonote_basic/cryptonote_basic_impl.h"

#include <boost/program_options.hpp>
#include <boost/program_options/options_description.hpp>

#include <algorithm>
#include <string>
#include <vector>

namespace cryptonote
{
//----------------------------------------------------------------------------------------------------
void address_validator::evaluate()
{
	std::vector<std::string> net_types;
	if(m_network == "auto")
	{
		// check all network types
		net_types.reserve(3);
		net_types.emplace_back("mainnet");
		net_types.emplace_back("testnet");
		net_types.emplace_back("stagenet");
	}
	else
		net_types.push_back(m_network);

	m_address_attributes.resize(m_address_strs.size());

	// move over all addresses
	for(size_t i = 0; i < m_address_strs.size(); ++i)
	{
		// check all network types
		for(const auto &net : net_types)
		{
			bool result = evaluate_address_attributes(net, m_address_strs[i], m_address_attributes[i]);
			if(result)
				break; // stop: network type found
		}
		/* Set network to user selected network type.
       * An mainnet address is an valid address but if the users evaluates only within
       * the testnet the mainnet address is invalid.
       * This behaviour must be visiable in the output.
       */
		if(m_network != "auto")
			m_address_attributes[i].network = m_network;
	}
}

//----------------------------------------------------------------------------------------------------
bool address_validator::evaluate_address_attributes(const std::string &net_type, const std::string &addr_str, address_attributes &attr)
{
	bool valid = false;
	if(net_type == "mainnet")
	{
		valid = get_account_address_from_str<MAINNET>(attr.info, addr_str, true);
		attr.network = valid ? "mainnet" : "";
		attr.nettype = MAINNET;
	}
	else if(net_type == "testnet")
	{
		valid = get_account_address_from_str<TESTNET>(attr.info, addr_str, true);
		attr.network = valid ? "testnet" : "";
		attr.nettype = TESTNET;
	}
	else if(net_type == "stagenet")
	{
		valid = get_account_address_from_str<STAGENET>(attr.info, addr_str, true);
		attr.network = valid ? "stagenet" : "";
		attr.nettype = STAGENET;
	}
	attr.is_valid = valid;
	return valid;
}

//----------------------------------------------------------------------------------------------------
void address_validator::init_options(
	boost::program_options::options_description &desc,
	boost::program_options::positional_options_description &pos_option)
{
	namespace po = boost::program_options;

	desc.add_options()("network,n", po::value<std::string>(&m_network)->default_value("auto"), "network type (auto, mainnet, testnet, stagenet)")("filename,f", po::value<std::string>(&m_filename), "json file name, if not set result is printed to terminal")("human", po::value<bool>(&m_human)->zero_tokens()->default_value(false)->default_value(false), "human readable output")("address", po::value<std::vector<std::string>>(&m_address_strs), "ombre-currency address");

	pos_option.add("address", -1);
}

bool address_validator::validate_options()
{
	if(m_address_strs.empty())
	{
		MERROR("No address given.");
		return 1;
	}

	std::vector<std::string> networks = {"auto", "mainnet", "testnet", "stagenet"};
	if(std::find(networks.begin(), networks.end(), m_network) == networks.end())
	{
		MERROR("Invalid/Unknown network type " << m_network);
		return 2;
	}
	return 0;
}

//----------------------------------------------------------------------------------------------------
void address_validator::print()
{

	char separator = ' ';
	if(m_human)
		separator = '\n';

	writer out(m_filename, !m_human);

	out << "[" << separator;
	for(size_t i = 0; i < m_address_strs.size(); ++i)
	{
		if(i > 0)
			out << "," << separator;
		print(out, m_address_strs[i], m_address_attributes[i], separator);
	}
	out << separator << "]";
}

//----------------------------------------------------------------------------------------------------
void address_validator::print(writer &out, const std::string &addr_str, const address_attributes &attr, const char separator)
{
	out << "  {\n";
	out << "    \"valid\" : " << (attr.is_valid ? "true" : "false") << "," << separator;
	out << "    \"input_address\" : \"" << addr_str << "\"," << separator;
	std::string address_str = get_public_address_as_str(attr.nettype, attr.info.is_subaddress, attr.info.address);
	out << "    \"address\" : \"" << (attr.is_valid ? address_str : "") << "\"," << separator;
	out << "    \"is_subaddress\" : " << (attr.is_valid && attr.info.is_subaddress ? "true" : "false") << "," << separator;
	out << "    \"is_kurz\" : " << (attr.is_valid && attr.info.is_kurz ? "true" : "false") << "," << separator;
	out << "    \"is_integrated\" : " << (attr.is_valid && attr.info.has_payment_id ? "true" : "false") << "," << separator;
	out << "    \"integrated_id\" : \"" << (attr.is_valid && attr.info.has_payment_id ? epee::string_tools::pod_to_hex(attr.info.payment_id) : "") << "\"," << separator;
	out << "    \"network\" : \"" << attr.network << "\"" << separator;
	out << "  }";
}
} // namespace cryptonote

//----------------------------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
#ifdef WIN32
	std::vector<char*> argptrs;
	command_line::set_console_utf8();
	if(command_line::get_windows_args(argptrs))
	{
		argc = argptrs.size();
		argv = argptrs.data();
	}
#endif

	namespace po = boost::program_options;

	po::options_description desc("Validate OMBRE addresses and show properties\n\n"
								 "ombre-address-validator [OPTIONS] WALLET_ADDRESS [WALLET_ADDRESS...]\n\n"
								 "OPTIONS");
	po::positional_options_description pos_option;

	desc.add_options()("help,h", "print help message and exit");

	using namespace cryptonote;

	address_validator validator{};

	validator.init_options(desc, pos_option);

	po::variables_map vm;
	po::store(po::command_line_parser(argc, argv).options(desc).positional(pos_option).run(), vm);
	po::notify(vm);

	// print help message and quit
	if(vm.count("help"))
	{
		std::cout << desc << "\n";
		return 0;
	}

	int err;
	if((err = (validator.validate_options() != 0)))
		return err;

	validator.evaluate();
	validator.print();

	return 0;
}
