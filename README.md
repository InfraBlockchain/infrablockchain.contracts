# infrablockchain.contracts

## Version : 1.1.0

The design of the InfraBLockchain blockchain calls for a number of smart contracts that are run at a privileged permission level in order to support functions such as block producer registration and voting, token staking for CPU and network bandwidth, RAM purchasing, multi-sig, etc.  These smart contracts are referred to as the bios, system, msig, wrap (formerly known as sudo) and token contracts.

This repository contains examples of these privileged contracts that are useful when deploying, managing, and/or using an InfraBLockchain blockchain.  They are provided for reference purposes:

   * [infra.system](./contracts/infra.system)
   * [eosio.bios](./contracts/eosio.bios)
   * [eosio.msig](./contracts/eosio.msig)
   * [eosio.wrap](./contracts/eosio.wrap)

The following unprivileged contract(s) are also part of the system.
   * [sys.tokenabi](./contracts/sys.tokenabi)
   * [eosio.token](./contracts/eosio.token)

Dependencies:
* [infrablockchain.cdt v2.0.x](https://github.com/InfraBLockchain/infrablockchain.cdt)
* [infrablockchain v2.0.x](https://github.com/InfraBLockchain/infrablockchain) (optional dependency only needed to build unit tests)

## Build

To build the contracts follow the instructions in [Build and deploy](https://developers.eos.io/manuals/eosio.contracts/latest/build-and-deploy) section.

## License

[MIT](./LICENSE)

The included icons are provided under the same terms as the software and accompanying documentation, the MIT License.  We welcome contributions from the artistically-inclined members of the community, and if you do send us alternative icons, then you are providing them under those same terms.

## Important

See [LICENSE](./LICENSE) for copyright and license terms.

All repositories and other materials are provided subject to the terms of this [IMPORTANT](./IMPORTANT.md) notice and you must familiarize yourself with its terms.  The notice contains important information, limitations and restrictions relating to our software, publications, trademarks, third-party resources, and forward-looking statements.  By accessing any of our repositories and other materials, you accept and agree to the terms of the notice.
