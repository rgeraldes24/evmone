// evmone: Fast Ethereum Virtual Machine implementation
// Copyright 2022 The evmone Authors.
// SPDX-License-Identifier: Apache-2.0

#include "../utils/utils.hpp"
#include <gtest/gtest.h>
#include <test/state/account.hpp>
#include <test/state/bloom_filter.hpp>
#include <test/state/mpt.hpp>
#include <test/state/mpt_hash.hpp>
#include <test/state/rlp.hpp>
#include <test/state/state.hpp>
#include <array>

using namespace evmone;
using namespace evmone::state;
using namespace intx;

TEST(state_mpt_hash, empty)
{
    EXPECT_EQ(mpt_hash(std::unordered_map<evmone::address, Account>()), emptyMPTHash);
}

TEST(state_mpt_hash, single_account_v1)
{
    // Expected value computed in go-ethereum.
    constexpr auto expected =
        0x084f337237951e425716a04fb0aaa74111eda9d9c61767f2497697d0a201c92e_bytes32;

    Account acc;
    acc.balance = 1_u256;
    const std::unordered_map<address, Account> accounts{{0x02_address, acc}};
    EXPECT_EQ(mpt_hash(accounts), expected);
}

TEST(state_mpt_hash, two_accounts)
{
    std::unordered_map<address, Account> accounts;
    EXPECT_EQ(mpt_hash(accounts), emptyMPTHash);

    accounts[0x00_address] = Account{};
    EXPECT_EQ(mpt_hash(accounts),
        0x0ce23f3c809de377b008a4a3ee94a0834aac8bec1f86e28ffe4fdb5a15b0c785_bytes32);

    Account acc2;
    acc2.nonce = 1;
    acc2.balance = -2_u256;
    acc2.code = {0x00};
    acc2.storage[0x01_bytes32] = {0xfe_bytes32};
    acc2.storage[0x02_bytes32] = {0xfd_bytes32};
    accounts[0x01_address] = acc2;
    EXPECT_EQ(mpt_hash(accounts),
        0xd3e845156fca75de99712281581304fbde104c0fc5a102b09288c07cdde0b666_bytes32);
}

TEST(state_mpt_hash, deleted_storage)
{
    Account acc;
    acc.storage[0x01_bytes32] = {};
    acc.storage[0x02_bytes32] = {0xfd_bytes32};
    acc.storage[0x03_bytes32] = {};
    const std::unordered_map<address, Account> accounts{{0x07_address, acc}};
    EXPECT_EQ(mpt_hash(accounts),
        0x4e7338c16731491e0fb5d1623f5265c17699c970c816bab71d4d717f6071414d_bytes32);
}

TEST(state_mpt_hash, one_transactions)
{
    Transaction tx{};

    tx.kind = Transaction::Kind::eip1559;
    tx.data =
        "04a7e62e00000000000000000000000000000000000000000000000000000000000000c0000000000000000000"
        "000000000000000000000000000000000000000000024000000000000000000000000000000000000000000000"
        "0000000000000000028000000000000000000000000000000000000000000000000000000000000002c0000000"
        "000000000000000000000000000000000000000000000000000000030000000000000000000000000000000000"
        "000000000000000000000000000003400000000000000000000000000000000000000000000000000000000000"
        "000001ba90df364951119f0e935b90ed342b9e686985fb7805f532c5432c2a46ba1233be5ed196ab7d467c8cc0"
        "73686342699c000000000000000000000000000000000000000000000000000000000000000000000000000000"
        "000000000063ecd7e7000000000000000000000000000000000000000000000000000000000000000200000000"
        "0000000000000000000000000000000000000000000000000000000a0000000000000000000000000000000000"
        "000000000000000000000000000001000000000000000000000000aafb72183a85a66ec7eec6a9d3374f3a06d8"
        "a25100000000000000000000000000000000000000000000000000000000000000010000000000000000000000"
        "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
        "00000000000000040000000000000000000000000000000000000000000000054c7bff9ff28e80000000000000"
        "000000000000000000000000000000000000000000000000000001964617c9cbc649c28b9710bbe61cc10e0000"
        "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
        "01ba90df364951119f0e935b90ed342b9e686985fb7805f532c5432c2a46ba1233000000000000000000000000"
        "000000000000000000000000000000000000000155f6604df131609d8058c7f0ad8bbdf96f4bb6b5cc00c96aad"
        "da6f61455681990000000000000000000000000000000000000000000000000000000000000001000000000000"
        "000000000000000000000000000000000000000000000000000400000000000000000000000000000000000000"
        "000000000000000000000000010000000000000000000000000000000000000000000000054c7bff9ff28e8000"
        ""_hex;

    tx.gas_limit = 387780;
    tx.max_gas_price = 1500000014;
    tx.max_priority_gas_price = 1500000000;
    tx.sender = 0x204cc644e26bdf879db422658edee62e302c3da8_address;
    tx.to = 0xacd9a09eb3123602937cb30ff717e746c57a5132_address;
    tx.value = 0;
    tx.nonce = 10246;
    tx.public_key = "f8a744e284bfb05b81439fcb8e55d92ba355776388dbe67e859a5dc558b34ed38a22541579e828e93cb0069a1281e7cd7f0ecb024114bffb0a8e7212b350531b62ceb6be7cd249e6a0e80c4ab42f4358fffe4a91f4d798be1ff5238e76d20834386cb40ad1979b2182c7cba90bf0e60ede7e3f93c9fc41db4885d62beb921f24e1ed514e5e8add0789d1a57dbf31b5d393435f45c5b9894b8eb3a71b8e7fbe235125672af4645350d343ec730bf901f0716bc37828ae736def9233d1373b414073a30635620f66f385b85506c3f9ce1ace12de641841084ea3ec53021506decadb4560762246da483b1f38364a942f5bb5da7f4d76b532a16a3d6e818f83489f98fc95c163a8732497d128dfb5172dae502f73e6be747118be43a82b907293361f33b2655e7d7a855066199b4d62882d5a01f7a9024ffa82d770158ec31c4b70748b4a109438e546cd6a932730dbf42e0d6eb0559dd8bb7ee710c7aaf5fdc7fac317da348ccac65f32076763a521856a29ed137589d69f9d1adf3187b30739f09ce1a62a92dbf0c692a078c340182e17869fc4b265883a69f7c99e9a577fbdb2d491f66c54e26909da073ba31f0c4edbaf48b7a10d9124db2236e8e9946c2bcce339ae03095865a0acbb3d72a5a3fe95d3ceeff13b723e4b7e426d2063c382f74439b3345cea4bd07b023bd631e21ee717a0382d5026b97c43e5e4e521037c1c3c230e99b4cab1c0f6818575acd9cf49af653617465f928dc74e09a9dea30781dfa31132eff167e0fab5defd0358788b934e892bd63fd86010f8a3217a2a4bda6932d0aea6117849f732678d370903e71271546dd98e6d2388af6fd5e4d3e049bb06dce644648afd5b2eeda4012f78edced50860dfdcb486e2e751f1e3730d0895cc4e2266e587dd3b4f454751629600f20a1f2f6601abd2b86b9646f2a7d351f9884e58edd6397e9258833a4c918cc4eb0410ccc055b6a45f0a22250a3aea467d0e22332d37c0c22e5951cc54e11e40191e5bb33300850822abd1b586fbbf7a3ce0113cb383bc096c2275f46b81f5d6ff8bed5366d41092af0b51961c477f93270c5b8ff5278cbb9dbb0957585fed0e6abdf4fa48ff573445c07ebc84590b4dde212c67f0ee880bd2ea831896eceef5e297c6575410a30212eb87a0ff257415b54565d34117024e185c04c426a9e38521e2da71886eec5db83722b713fc2519da44a5278bfd47f297134cdc30e856b879d4e57f26d85798f1ce63566e920279d7f1b78abb48aab0f41f4217f8c55c53a01eb512a9d2ec2f56c05dfabfd0b40e065f1bd430412f97496d029cad7252bc8aac2f39720f06994e982b1cecb759a6e74be55dabe06f41bb41d284b401bf1b3179948cdf2776d67f3d19aec2532ebb1d3402be7966873bd54a16fff19fd9eb489cf5a58e567db2ddaddf0aed8dbd964c98355c684378efaa014cbb7a7e0e125f790c411fa2b2322a63d29cee3afdac23185b1d50973ad46f35dcd840712a28d1c2dfb0e601d6202ffcc80a1806db004494df78e9d02f7bc0715d37e226c2a7a6ffd7110f32588ad3576017ef8cae7427113be87213deee64fe103d2192ae1ced91516353e3a493759f648b87351eb204229b892675b2681b97e69ae4d82f0ec3fc0e128db887cfea46b5224595eb57612b2df2c9beddeed2f499af8ca212aa5e1aae6e0c8ff96bbc906f12426fa5df5853adb6e4ba6f9db0ee29c07c9aa43e1075bef57d5ee68af97a61923c5139edcff779abfb0092dc82a765ad9a3c8a1c679f172290aa8da85a3200fcb3dd1d38bf45ed24829bfa734a8b5ac2b7d9a1adf06c65a82b6bd4b868194e0c236a744d677b3a910f8b3cf36f034954729f776d000a93659490dca73419ee503a3800a22b07b4ab59213e353635d9dbb617b78d6270eb95ff798fb9654fa64e02b370a367380147a3ad11fda876f080a5ec1c3fb9e36d48d68039c4e4cb3e7d132c4653b06e077fd55409e2f0f75b08585cf157af386a32203fc9cb6b67d948ae70241a0d8ffd3d85648ec20dece034b1d6d1cbdd576474c55bfa87a70656fd07a8576f25324666a4d113cc24ddb9d9592d5b04590864ab8e802816c7280c4ee82783ddf9de56a16c4ab3f36f0828d6bc6bf95f169848c868032cbc793a6c2c696e9ec3138384d34661db11a3841e598864c3d83632e7ccf0b07f229576d53196003540b06a84dc4ac37acd67571b3d97dba79c51df709d75a834bc5998998e3b18cc76e7f4fb7f2401b1ff5e36a05a64cfd7fe57ff57473daf4503a9570a6d06a73e8c1b82bd2e6784ee28cfb9066db6cc2c34cbef234aab1edf5937a9ed6644be3fea45c57058b03ac15ed2321f922550c193fe7cc5cc55ac91a3374a6c835a2a47089cef5ada65c0c60ad2b356bf69c981a48b7d0d97238cc191813318d866935b4108a4f59bd9b3238c198999b528cab39433e5dc6b7e17b334ec8e6c7e09c7683d985345996acdd90afc3ed292dd96c041504d3782ebfbebfdc1ba49b4236979e8b6317de9bdedff107066f79be16fad2ff8268710fdd0b2b06346ab574ada3e55184e4175237b1c413a7cda2098d8ee39b6ee96dc00b57c0a939f4426ee6d648217fcd60bbafebdedfedbd13bdcd0a658fc2d76264d10db16c57a402e7f38def00260f9208945db84d13ff1d908e4bdb3141e89820ce6ce41bb5922e9f4ac975bb660597502145e308c0f77ca2d0c0b8efd663fabb81ef079bd39bc39d440bd368eff99c670b43faa5c387bb4fcbbe9eceb3b75aae7834f58ffe9d176a3943f64eeb8e5b1606d7a4c754fee95f91137edd3163153618286a93f0a39d8ab23de7993fc4617e545f5146414c8cdf8426d829f33e85c2fd7070b7a13ed63df1223e34f3272ec8b2a18ed7b4a162e3d9fd8cc79de8630b7aefe290b386ed973dcfc5047f42dff2585aad3768cd74934d7e70c630dfb52258c2dd3f1df1bf2315331925dbc30eb903bf54c5b7b915c98f61a68513c6d4f2c54cd285d6d9cf65a8aee5f19243c3a36090c8b38b3f1eaa33687cde04f6427f113cf21ece5990495f2414a6dc3b4f297faa12b577ea275cc2025a6af13e220ff47da486ca4e66ef5715bd141c3eb8e05c783ef578ce83ecd41d006a604ec5384ba9214e87226e0f1a2623ead698feb59e868274c5d6cd48dc7b6a52e1a3e7caa1d8426c5138c31e43452d213898b25adc8553da8489728746fdcf04f083eb4f93ea9753d968e2d442e406ce3cdb8d8d47321d4b8bce10110112d7d31b7418b77619f92b73713fa3afc5a46924c2e1347d7de262f3864101bc94c1c9fbde6d33f53142d61453bf5658a7f565ddf66ea83c2c2ba63b96ee7daab2a9f5bcfecb57e983f4c0eb39d25b8e75516cf79a6234a4b26a753dabc906498fe1aec5d9a8397c7635560a0f66532847907ced50403c2c162e17c542db9760abcaca69c76df5008d4f846c0ec224ca3740442d7d320931608091b02a63ba8e4941f589e74d1d306e7df420f451f139f0b8460e84df9ed0ec4bb3d150a2cbc424b69fcab142a0373535f13cfe374f65420e82659134a1b522c47b6110dba3a0e38e5a94abdfc65f6256fc30d1a2883770b1ba9f5a1210e8fe4b7b0e112ff2d3e58269da655969c3f82184a1c579c76303967"_hex;
    tx.signature = "8ceb6441f6766a26bd5837f6e0491f48ff17da2f72cab352fd1bc6a360c36be1cc87f6860551049325d20bdee696d5e08911b010295da79ce6aca9367d8804697e908cbe01d59bcb501b0c364c1349c4efda68234b11840b82492578d5031ae6b2eabb77496b95bcf7125a87a2daa1e99f6c686d5c0a7a69341c9e2c4a1ec2d842fad867c23230ccfb52dc0a9e5488b8d0a1ec85751df10d6a471f216a148699009942e322ad93bf2ca3d38005cda4b054558bacb39ea207535abc10423ade1574013cfc056345687c5c9ee4821cc8b97b99718bc40eb5965f3d2a1bf9a1c6c4a86f62237a7ae0785ce314ba294a46f9a4d471acee9ee6afc6bbc19c799996947b59418ea5ca719d9d1a22fe834ad17ce94ff485141f14a83d44e35bc9e1b79b4c256b2feb80d6c20efe4f66b5067c4460a300cbd95405febbd233f52c09b488206665bca86f6eb9478752ad9254c00c861ac61bd74a93c869a4993e1b115ace59ee7e340f24c818920af37f4550bb4b7e34984e7f096bd17b2e07f6b108840fdc87beff485f0d48050d06d30d666a59fdaf9a27ad19e0f57fe687ed661aa16fe68b597d2712204a3fe7b26efc738e5f29dead08f30bdf2a3207a63fa02f48fc4b7ba19a425037ed33c9cadc43e1cb58a169b0e2a220834e7c2544da02a003bde51db2a7fd2794d59569b3b60f462c32a0694b2dcee57f27e1290954af7277ec63171d8da5b7c348f1b1ad2ebef92ad872fa88740210f57b20c989a1db4bf21183ed36c8ab6faa8533de2e64676aed39ec9820b60f0dbeb12077a01c915d2ea9c44ca8de7f1c748302cf75e778049e8658d031f1d45292a800556c50bbbec573abe201b33152bf21b3d1d29172fb9cd1eddc81ba22867460f0d7b7c28715d7c49f7ce39b669313fea9701eaffddbcec714750ac5e9dfc178623d729572cb4bce8ee4e92f680a5179f48e1f34442910f21b692f10fe920a489df9f7178350df099302cf1be6a6a4f55c6a39cf18b5eb30d3418613609b2a3772a8f1c296067768ef48d79f0c794ce0f24ddf94ea472ac7f55d643c42b7fb18739f2d529eceef6a2235d5f9f83808845c203584b72926b65dbcac64975cea8cd1d30df56cb5ec1a2325be9e594aed47a860dfe19c4a4738069347d8f35971dac9c8877556332cca36da4ebb064487c8ed411d9a8acb08b3146b469c04e05cb6a2490aa7e9668a91666cda0fc4951fee13194f88c480318dc68c38ec0c6cb01f05ffec4ed0e0dfd2303dad188496de3fea7ee4194973fb9570480eec10f52ba713e10eaed33e8185e79542601705eff4b45eda8afd2fb14f0e9f3a4594de4b751872e47c4b9d0acf3fb51f49716145197755970da06c63b3383e4376dac812b6995ab72ec5d431fd9fb3eb3e78e7a558da8a54c0f12d71e28eaa5cfc599da70932eafc0f65a77c1fd32616b5a1012f208759eb1ac69414680ecacd0acb37bc40a10b95c2f1c72647857368eef64f229d7b7ca02a43f15c186ff80523df671b2a0f9ba58fbb6a44e149cb411d61eb830fdb0ccdea2d5088b032114e0e3b8bcf4d67def743fa87ecf92508373b88de05705b49d74301250028ed0f8f827dd87251f1e490eab04f7b4e56b401170cc3d96fe511589855145f129b0eb2432a9011085d7195d237955a973a6634a156c3960f2960e0d5b73d5ac7d3953c2ddb7c79fb35ebd772b63c56e6e98a422895875fc3898b8783223eb599abda9aa3f43b41103de99166e06b7414157e383ea466983fb7c2aa17c0f7c78838a0ad45156984baa977c144a7dfcc858ddf4467e108aaf98a4f652e95ee3efe83b058db7026d395e37ffba61cf69251532dc2a4d753d139c55b537e3fa0cc3ef10a256b574dca53e8ac530766bed5995b98d1f66776c8fe5ce8725633168dd0242635a60c15059404e432c79f977d9ec17568bc49a07d5fa85c23293211b6d81dff0b795e0ae847d41f2d24fc7a38dc484e2517830d7b57cead20938ae8d4bec21f1ea5e2ff0b1d174ef746c299139fccc7d3bb9da1a2a69175c0b0b1fafd68e082de4eae12a705e7c430abffb400c7612c3595cc98f1fc6b5fac8f328d6d4e12f134f37e6842c9fb50d8767da5c992c6e353b75717a94318a14432d0f56c007f59b49e05cec3561f4e05ce84f9b7d0fdd7d18debe8be8009114feb87ca1f40cb8e427a007e32f9bde19f2187ffd22411595b8d70b16e9761bf3ef5d2bdbcb2a134d29006365113c73e6e317772b089587dcee59f07a3485c4d977ae1c4b20ae4f1233dfcd47fc198c7267294ea133bee66d7f9252e542bde07d59c0dec623d63c863223291bea9a58070d6799803fc7f039f4af77fb72f239f9603380c1870f67c42d2f1e81f9f39d70d9b7e54eb4738dc2f556eb990dacab63a1171774d427441d18ff72e50ef84689d5a86ce21d4b312b1a68d2dbe90cdb30975c903c385466e2fe7264f0a577a056c787fe0735d297df411a774efb9508f5faead7de6966344ff79f819c8256d236a0b620a47472c3f8f85a70355f01f2e9967a3fee532cc2a6a8a688ad564d267fa1cd186a1576e9990a7e19d4ecf480d59eb875dcbd7f6824858f5cbcc6c859c75ad0b1fb6f7534e89d1f1942aad09750590fd5e3104561c265b12f753bf438ac3a505e7b43cfefe518b725829cb7414f175e63328092b7cf74817606d63318f6d2ad4f8b5b8031575c9a6011f261e8d780a8005f29388c18cd211f73b3032e58299fe35914bb190690c83989b7225baf80f5b566e993d9dd463670afb3aebabebc35b5a9f18dcb33f0f48234e8b13777dc232e57f7082f096f156e66166622590d2192c95c3c3bd4679a8537c6af61668510cfaa59020ba9dc28bc6cb1e8df56c0c968e50ec65d00a1539f9920b6118bccf54993df9dfd961ff8f299e321731795b4a9b7eb9da6d2dda767cfe2687594595ea53e96b4d7184598a74341d903a6e3534d177c57856c2029fbc2ce81dd83fc55c33679cff6cd965ffe63377be58acc2a26b9741439cc60ccf7cb3b575d6714279611907181bb01f2b38623c0a4399599ced786c4086209aacf83a72366d163088337443102b3cf3b4ce2f32075bf2d7761b64f9f0935e91c277c47d025130c5102dd7333edd51fd1a88bb64bf62e4b4496f8c6e679e9dc4ee9c4cca3fe977040e471f73e565fb5e0c8d25a9e26c30de60666ec03ff30edce67c914e597ec29fe7155f71c9535686c9a6bd9fccbfb26ade706523c238b1a49ed233604101a43a3ecf2833c74c703b2bbcf825bad1bdac0ffbe5ea4e92a98d1b1712d04dc829026e4c0b510b87ea347f7d2b52a14a4ce55ec2785a42e311936be2a082c19b497ceb77092b385fd4d036e6d673087fd4f1d9ca347f9cd7345791411f189c016cea21d5eaa24b75a70cc0442540051eedeebed7f19bdd54501bea069aa754f0be80fdcdbb503a89c528b60563b5c30c6452f9d259190b85ca4595d59ec122332f2439392df180ff292dda8a8aa22837c1493bacadcc423b158f0a058b0bf26a8f810d9b9a5f90a081cbd51058547d1abedff46cb8d9c8394c12b58ebbf59e1d372fb108bbed011b8f438b4a758e1ecd8f44bd999bf732e24c9309455aadff5b8cdab986f7b7455a97f5083b03ab27e77d703dbe2f184a6fad64c5d34f91cd9cd213d4b72e5989567cf2625bd55dc219c29b91446c1483af33db8528b98fe00aa48b606b6394f6510b835e7a1e07279ab4b46f7a24ec40900afab2547759e45c5bb4225da6435ce17c2e3105237211711398d323828ae74c7aa4fef63ce65c936c62ce40abc8b756911ca65998d33d25745526b5e00fb444b512248b573b1e025dd95c02512b464c730de09449b4f32fcd859605671d49d9acbb1f9e1c4f16a471a7b2cfc911c03cb6dba49643460947d14885b78a7579c464d46c4f8cb5f7b84fee505a4f8d095cb702cbd2558944fa6e7900dd5c67a22e89e673a151819bc52c8ab30e43d59fd92979e651813d8961214d05eca0261685733609f04fb71ed455f3266be47420fcf2c1a08780e4b7bbbad3d9a918e824b0899772cd6292dbed6c7d7fc3e29b85db834ac71618d31680cabfa78e0856da911da17508112a08541584c791f2ed45deb43611dc4db8328349d95282d6bb6a1728ef09a985ae3ce0f65e8e067c35352b8ef277bb730dc78cf31e2c7f1e42a5a755cc9dca423164b659a9877eec1bef3a047e1a0403bc83c09172322d1a7c380ebe8de3893e859823309bb7c57150be76049d63267277b17349771bd23b3e880769b15d55e1fc08a2bf2628b79d2869fef1b75200d02e78f572b45aa546f50b9286dbc6311980ef3318952230f2e3a26fa7d08ea43c00ce61fab3db5aeabb93282a58334338b7f39b46cfaee18862e81122272f33b0eb40b966baea32ddb7571c3f90eb58dbcdeb1de8ea153575975b28efb7742e3ffbcd65ce495c0475b600541ca9eb2f8338f4b047fb9ca05f9404d839f579ece370924be86a4d7486bfbafd3056f8ec0720a6d84b99300da35b0679e0b957b376467e90b7a71cf6ac81eadd30b9e1de7305087d70df651760de5647edb172c4f92a8fe3c0f5dd75e457d9d4d697c880a63c4fd94f95f6eadde9979bb4d2327bc49b75d554a028a1db6e9de7fb6c2aad6a3e5be29e0619f0f26fa7ed815cae960d9f814786b153f9a78a596346aa27765864c4c6a7352d3597eb2f9a130591b7a09d8e040056e9aaf9d2986b25458f18e04cad0523b3004081f2b8fa2918c5749e15a1b801689745ad935a81e7ef3445d5d5fd4735e21c668ac3c95e2d989c9b78914a256a841353e841fefb50e8475a677d984199f3ebe2c50d5ed95b4814af1dbee6e83558e4e8bedafcb67532b471fd96b206f7113eb2ef2ec51de8106d26435f02ecefb5a4baf597dcdc1c7a4394ea6ee6b2655ca6f7457d07f13196b1a961c4da1c4d8db1b497eb38d6977f16ac4e351bc8e5fbb32732bc53efbceb36ae39c56308ca9354eb23ca600c6217bb778d3cd001a34551062b5f34d71578301acc3e4dfd8148be2f94b5ff8d2e3bbbe719e47ae51bf3f498fcfb6481b61e01c59895bec9c3f2864af2285e58ef56c7d54115af96a9ac11eb0e7f2027c1266aba0f3e0845e40f2958ac8db41760fba3688678bfc4f852f78f92c175693cf24d3a37031e27fb4435411d86ef739fd05af1beffef7d30f6e6bbd7bde02cce4b362a56ed83a45f862f7a4540169ad9e48a6b28012fbc133948a077ef4c170b6b2ee70329b0ee38a3fab2e289883e9865af44e3d7972e1898e01aeba282da0d525aba54af74e18749aca708d845c62c6ae217d2032050a27f0b84a6841e3a1be68e72dbe4011e44761c9fa82e350a0755663bdbba393731b0cd4c45206320097b11e4c497b642044646645302308d07df8db9a2f6a37395409babf8e37b973c760fd7d2e66a77de23509ac36d3a11fc5fe35fe723258d442713de8ecb3490f649b919daa2a884addaeb350b8f6280411ce63d84186ffbf940c11f6d39255b9c80d87caadff23fb58a9d67179a7e84320ed944b157b976ef04fa1eb5456a4f353b6de6f768d084b36f5d5d8583da91dc83ab253aa23743f286bfc85992bde0b60f260c4c30474ef5fdca981f6829a6828a9417ea5c867f2e704b4a016547ff0f35b476946c8acf7d537dcf5f606a81ef8ce74bf91216ffbd5130925ce91d501eecd3b23ffd66762a59d0ce82e86287c1f7898c17f2d9390bc5a4e0fdbd68e07f14a680cedd5479e5d454225ff3b1812f6d1e7943947fbf042f8ef6e9012fdb8db66956c5f9211d0dd84ab6d6d2e42b74b84d1eecc9463c5a132934bce956ce80f44d0061142616ade4067feb262dca53d9948f68fbe60f29cec71dbac8f1f666055cb78b7ddf0f471304c26adc95778028fff723c3f15f042193d8c8a767d6c24bb5b9fae70a3bdd82511ab64f5d68eda33523e82347c4f955674469f1208daa2fa20375c334422ccaec754e8f96ad88eb7a60aaa697de6c445ff369df818930b4f26f15daa240931bde222ee0f8b6d790c40b01d6586bcd3f230e9b2bf21d691289f2bf6b566269a387e4c90f5e432a3085783a45c652c42460ed922f1577fd047559095b33d345927da2e291f6a041451c23d614144f057f162850970c4e8bdca9a1699833a31ed524817791581f372e27bec3990fc2b7d0c66ff239c8a357d9e94a801f2f7a0567a7e1361797dfde03cdaa273970f07243e257888742de9770e23df6258e6cd08d16a9a0a72d734583101b767019f7f72f4607aaed93c03b25c072e09a44c4a8ebc8a304651569edc6ecde7abb727109ed77fc9f279be2a687977293231f97137328fa05902c34525e7ca8bfd2eaec08758bb6c81c3b4f696c7acdd91a2b4c75cad5d7d801909a9da2b3104673c7d4e1eaef001c546a767eceeb000000000000000000000000000000000000000000000a0f171f252d2f35"_hex;
    tx.chain_id = 11155111;
    
    const auto tx_root = mpt_hash(std::array{tx});
    EXPECT_EQ(tx_root, 0x9eae8bcf25335b22d7ebff8c179efc8e8dbcaa55aadf331d2efba1cf7cb3112f_bytes32);
}

TEST(state_mpt_hash, eip1559_receipt_three_logs_no_logs)
{
    TransactionReceipt receipt0{};
    receipt0.kind = evmone::state::Transaction::Kind::eip1559;
    receipt0.status = EVMC_SUCCESS;
    receipt0.gas_used = 0x24522;

    Log l0;
    l0.addr = 0x84bf5c35c54a994c72ff9d8b4cca8f5034153a2c_address;
    l0.data = "0x0000000000000000000000000000000000000000000000000000000063ee2f6c"_hex;
    l0.topics = {0x0109fc6f55cf40689f02fbaad7af7fe7bbac8a3d2186600afc7d3e10cac60271_bytes32,
        0x00000000000000000000000000000000000000000000000000000000000027b6_bytes32,
        0x00000000000000000000000038dc84830b92d171d7b4c129c813360d6ab8b54e_bytes32};

    Log l1;
    l1.addr = 0x84bf5c35c54a994c72ff9d8b4cca8f5034153a2c_address;
    l1.data = ""_b;
    l1.topics = {
        0x92e98423f8adac6e64d0608e519fd1cefb861498385c6dee70d58fc926ddc68c_bytes32,
        0x00000000000000000000000000000000000000000000000000000000481f2280_bytes32,
        0x00000000000000000000000000000000000000000000000000000000000027b6_bytes32,
        0x00000000000000000000000038dc84830b92d171d7b4c129c813360d6ab8b54e_bytes32,
    };

    Log l2;
    l2.addr = 0x84bf5c35c54a994c72ff9d8b4cca8f5034153a2c_address;
    l2.data = ""_b;
    l2.topics = {0xfe25c73e3b9089fac37d55c4c7efcba6f04af04cebd2fc4d6d7dbb07e1e5234f_bytes32,
        0x000000000000000000000000000000000000000000000c958b4bca4282ac0000_bytes32};

    receipt0.logs = {l0, l1, l2};
    receipt0.logs_bloom_filter = compute_bloom_filter(receipt0.logs);

    TransactionReceipt receipt1{};
    receipt1.kind = evmone::state::Transaction::Kind::eip1559;
    receipt1.status = EVMC_SUCCESS;
    receipt1.gas_used = 0x2cd9b;
    receipt1.logs_bloom_filter = compute_bloom_filter(receipt1.logs);

    EXPECT_EQ(mpt_hash(std::array{receipt0, receipt1}),
        0xb2863204ad0580dbec14fd35f8a0ec71fb179765bff7fc279f05349733eb627b_bytes32);
}
