# EOS Blockchain Contract for IoT Devices

This EOS dApp manages RFID tag data such that proof of location and time of RFID scanner events can
be used by novel solutions to address the needs of supply chain, manufacturing, access control processes and more.

The tag data is accumulated into an EOS RAM-based FIFO queue that is limited only by the amount of RAM allocated to a device account.  The tag data is incorporated in EOS blocks within seconds of submitting a transaction to one of dozens of globally available API access nodes.  These same API access nodes may be used to retrieve your scanner device's data with simple RESTful API calls.

EOS requires that blockchain resources are reserved up-front.  Once the resources are claimed all transactions are free.  There are no fees to use the system!  it is possible to increase resource allocation by staking more EOS tokens at any time.

This contract is currently deployed on the EOS mainnet account **eosiot11rfid**.

A demonstration device account is available as **eosiot11node** and setup with the correct permission to allow public submission of RFID scanner data via the **submit** API.

Open source RFID scanner device software is available [here](https://github.com/EOSIoT/rfid-scanner-node), enabling you to build your own RFID device scanner. The open source software contains the private key of the **scan** permission for the **eosiot11node** account to allow anyone to test the blockchain RFID scanner system.  The private key is linked **only** to this permission, which is able to **only** transact with contract's the **submit** API.

**eosiot11node** is the only account registered with the contract.  Should you wish to use an account reserved for your own device, let us know and we can set that up.  The resources staked to a device account will determine how frequently a device can send a certain size payload (bandwidth), and how much data can be accumulated in the FIFO.  At current market prices the cost is very reasonable to reserve bandwidth, CPU and RAM necessary to support an active RFID scanner with several messages an hour.





