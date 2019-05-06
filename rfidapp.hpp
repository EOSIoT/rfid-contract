/*
MIT License

Copyright(c) 2019 Evan Ross

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/**
* \file
*         This smart contract "dApp" runs on the EOS blockchain to accept and
*         manage RFID scanned tag data on chain.
*         The scanned tag data is placed into one FIFO queue per EOS account.
*         The data is available through standard EOS RPC API calls.
*         The data for one account can be distinguished through device ID.
*
*         This contract is written to an older but workable EOS contract spec:
*         https://developers.eos.io/eosio-cpp/v1.1.0/docs
*
* \author
*         Evan Ross <contact@firmwaremodules.com>
*/

#include <eosiolib/eosio.hpp>
#include <math.h>
#include <string>

using std::string;
using std::vector;

class rfidapp : public eosio::contract {

public:

    rfidapp(account_name self) : contract(self), _scanners(self, self) {}

    struct scandata {

        uint32_t scan_time;
        uint32_t recv_time;
        uint32_t dev_id;

        std::vector<uint8_t> tag_id; // 7 bytes

#if 0
        // Non-default consructor now allowed.
        scandata(uint32_t scan_time, uint32_t recv_time, uint32_t dev_id,
            const std::vector<uint8_t> &tag_uid) 
        : scan_time(scan_time), recv_time(recv_time), dev_id(dev_id), tag_id(tag_uid)
        {
            //tag_id = tag_uid;
        }
#endif
        
        void reset() {
            scan_time = 0;
            recv_time = 0;
            dev_id = 0;
            tag_id.clear();
        }

    };

    struct stats {
        double min;
        double max;
        double var;
        double mean;

        void reset() {
            min = HUGE_VAL;
            max = 0.0;
            var = 0.0;
            mean = 0.0;
        }
    };


    /*
     * A scanner structure manages scanner data for one account.
     * Typically one account is assigned to each device, however
     * we do provision a "dev_id" field in each submitted scanner data item
     * so that multiple devices can use one account.  Suitable network
     * resources must be provisioned for each account to support the number
     * of transactions expected of the deployed RFID system.
     */ 
    
    // @abi table scanners i64
    struct scanner {
        account_name  name;

        stats latency_stats;

        /* FIFO of scan data.  This is capped based on the ammount of RAM supplied. */
        std::vector<scandata> scan_data;

        uint32_t num_transactions;
        uint32_t time_first_tx_s; /* Time the device first came on-line */
        uint32_t time_last_tx_s; /* Time the device last submitted data */

        scanner() {
            reset_state();
        }

        void reset_state() {
            latency_stats.reset();         
            scan_data.clear();

            num_transactions = 0;
            time_first_tx_s = 0;
            time_last_tx_s = 0;
        }

        auto primary_key() const { return name; }

        /* Accounts/devices with highest latency data */
        /* Accounts/devices with most transactions (i.e. most used) */

        EOSLIB_SERIALIZE(scanner, (name)(latency_stats)(scan_data)(num_transactions)(time_first_tx_s)(time_last_tx_s))
    };

    /* RFID scanner data
     * View this on the chain with:
     *    cleos get table <contract account name> <scanner acccount name> scanners
     *                    <code>                  <scope>                 <multi-index-table type name>
     */
    typedef eosio::multi_index<N(scanners), scanner> scanners;

    /// @abi action
    /// Create a new scanner data storage table for the provided user account.
    /// An existing account must be provided.
    /// Fee: 0 EOS.  
    /// User account must have necessary RAM, CPU and bandwidth resources staked.
    /// Returns: "Receipt" containing connection string for placement into RFID device.
    void newscanner(account_name device);

    /// @abi action
    /// Create a new account with the necessary resources staked.
    /// Fee: 0 EOS.
    /// Requires necessary EOS.  EOS resources can be polled via getresrouces() API.
    ///  Parameters: 
    ///    Amount of EOS to transfer
    ///    New account public key
    //
    ///  EOS is consumed and allocated as follows:
    ///    1 EOS - fee
    ///    RAM (see resource allocator)
    ///    CPU - nominal - 0.1 EOS.
    ///    Remainder allocated to Bandwidth.
    ///    
    ///  Returns:
    ///    "Receipt" containing connection string for placement into RFID device
    ///       including unique and randomly generated account name.
    void newaccount();

    /// @abi action
    /// Reset.
    void reset();

    /// @abi action
    /// Submit scanned tag data to the platform.
    /// Parameters:
    ///     Account name 
    ///     Device ID 
    ///     Scan time - Posix time: seconds since epoch UTC (standard EOS/node Date format)
    ///     Tag UID - vector of 7 bytes.
    void submit(account_name device, uint32_t device_id, uint32_t node_time, const std::vector<uint8_t> &tag_uid);

    /// @abi action
    /// Show version.
    void version();

    /// @abi action
    /// Determine network resources for a given set of conditions.
    /// Note: resources required to execute this API will be charged to the account calling it.
    void resources();


private:

    /* Instance of the scanners table with default code and scope (_self). */
    scanners _scanners;

};
