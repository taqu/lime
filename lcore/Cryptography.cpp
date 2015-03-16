/**
@file Cryptography.cpp
@author t-sakai
@date 2014/01/06 create
*/
#include "Cryptography.h"
#include "CLibrary.h"

namespace lcore
{
    //----------------------------------------------------
    //---
    //--- PKCS5
    //---
    //----------------------------------------------------
    // パディング後のサイズを返す
    s32 PKCS5::getPaddedLength(s32 length, u32 blockSize)
    {
        const u32 mask = blockSize - 1;

        s32 padded = (length + mask) & (~mask);
        s32 rest = padded - length;

        return (0 == rest)? (padded + blockSize) : padded;
    }

    // パディング
    void PKCS5::padding(u8* data, s32 length, u32 blockSize)
    {
        const u32 mask = blockSize - 1;

        s32 padded = (length + mask) & (~mask);
        s32 rest = padded - length;

        s32 paddedLength = (0 == rest)? (padded + blockSize) : padded;
        u8 value = (rest == 0)? static_cast<u8>(blockSize) : static_cast<u8>(rest);

        for(s32 i=length; i<paddedLength; ++i){
            data[i] = value;
        }
    }

    // パディング前のサイズを返す
    s32 PKCS5::getLengthFromPadded(u8* data, s32 length)
    {
        u8 value = data[length-1];
        return length - value;
    }

    bool PKCS5::checkLength(s32 length, u32 blockSize)
    {
        const u32 mask = blockSize - 1;
        s32 rest = length & mask;
        return (0 == rest);
    }

    //----------------------------------------------------
    //---
    //--- ARCFOUR
    //---
    //----------------------------------------------------
    ARCFOURContext::ARCFOURContext()
    {
    }

    ARCFOURContext::ARCFOURContext(u8* key, u32 length)
    {
        initialize(key, length);
    }

    ARCFOURContext::~ARCFOURContext()
    {
    }

    void ARCFOURContext::initialize(u8* key, u32 length)
    {
        LASSERT(NULL != key);

        for(u8 i=0; i<ARCFOUR::StateSize; ++i){
            s_[i] = i;
        }

        u32 j=0;
        for(u32 i=0; i<ARCFOUR::StateSize; ++i){
            j = (j + s_[i] + key[i % length]) & ARCFOUR::StateMask;
            lcore::swap(s_[i], s_[j]);
        }
    }

    s32 ARCFOUR::encrypt(const ARCFOURContext& context, u8* data, s32 dataLength)
    {
        LASSERT(NULL != data);

        s32 i=0;
        s32 j=0;
        u8 s[StateSize];
        lcore::memcpy(s, context.s_, sizeof(u8)*StateSize);

        for(s32 k=0; k<dataLength; ++k){
            i = (i+1) & StateMask;
            j = (j+s[i]) & StateMask;
            lcore::swap(s[i], s[j]);
            data[k] ^= s[ (s[i] + s[j]) & StateMask ];
        }
        return dataLength;
    }

    s32 ARCFOUR::decrypt(const ARCFOURContext& context, u8* cipher, s32 cipherLength)
    {
        return encrypt(context, cipher, cipherLength);
    }

    //----------------------------------------------------
    //---
    //--- AES
    //---
    //----------------------------------------------------
 /**
 * from rijndael-alg-fst.c
 *
 * @version 3.0 (December 2000)
 *
 * Optimised ANSI C code for the Rijndael cipher (now AES)
 *
 * @author Vincent Rijmen <vincent.rijmen@esat.kuleuven.ac.be>
 * @author Antoon Bosselaers <antoon.bosselaers@esat.kuleuven.ac.be>
 * @author Paulo Barreto <paulo.barreto@terra.com.br>
 *
 * This code is hereby placed in the public domain.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHORS ''AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
namespace
{
    static const u32 TableSize = 256;
    static const u32 Te0[TableSize] =
    {
        0xc66363a5UL, 0xf87c7c84UL, 0xee777799UL, 0xf67b7b8dUL,
        0xfff2f20dUL, 0xd66b6bbdUL, 0xde6f6fb1UL, 0x91c5c554UL,
        0x60303050UL, 0x02010103UL, 0xce6767a9UL, 0x562b2b7dUL,
        0xe7fefe19UL, 0xb5d7d762UL, 0x4dababe6UL, 0xec76769aUL,
        0x8fcaca45UL, 0x1f82829dUL, 0x89c9c940UL, 0xfa7d7d87UL,
        0xeffafa15UL, 0xb25959ebUL, 0x8e4747c9UL, 0xfbf0f00bUL,
        0x41adadecUL, 0xb3d4d467UL, 0x5fa2a2fdUL, 0x45afafeaUL,
        0x239c9cbfUL, 0x53a4a4f7UL, 0xe4727296UL, 0x9bc0c05bUL,
        0x75b7b7c2UL, 0xe1fdfd1cUL, 0x3d9393aeUL, 0x4c26266aUL,
        0x6c36365aUL, 0x7e3f3f41UL, 0xf5f7f702UL, 0x83cccc4fUL,
        0x6834345cUL, 0x51a5a5f4UL, 0xd1e5e534UL, 0xf9f1f108UL,
        0xe2717193UL, 0xabd8d873UL, 0x62313153UL, 0x2a15153fUL,
        0x0804040cUL, 0x95c7c752UL, 0x46232365UL, 0x9dc3c35eUL,
        0x30181828UL, 0x379696a1UL, 0x0a05050fUL, 0x2f9a9ab5UL,
        0x0e070709UL, 0x24121236UL, 0x1b80809bUL, 0xdfe2e23dUL,
        0xcdebeb26UL, 0x4e272769UL, 0x7fb2b2cdUL, 0xea75759fUL,
        0x1209091bUL, 0x1d83839eUL, 0x582c2c74UL, 0x341a1a2eUL,
        0x361b1b2dUL, 0xdc6e6eb2UL, 0xb45a5aeeUL, 0x5ba0a0fbUL,
        0xa45252f6UL, 0x763b3b4dUL, 0xb7d6d661UL, 0x7db3b3ceUL,
        0x5229297bUL, 0xdde3e33eUL, 0x5e2f2f71UL, 0x13848497UL,
        0xa65353f5UL, 0xb9d1d168UL, 0x00000000UL, 0xc1eded2cUL,
        0x40202060UL, 0xe3fcfc1fUL, 0x79b1b1c8UL, 0xb65b5bedUL,
        0xd46a6abeUL, 0x8dcbcb46UL, 0x67bebed9UL, 0x7239394bUL,
        0x944a4adeUL, 0x984c4cd4UL, 0xb05858e8UL, 0x85cfcf4aUL,
        0xbbd0d06bUL, 0xc5efef2aUL, 0x4faaaae5UL, 0xedfbfb16UL,
        0x864343c5UL, 0x9a4d4dd7UL, 0x66333355UL, 0x11858594UL,
        0x8a4545cfUL, 0xe9f9f910UL, 0x04020206UL, 0xfe7f7f81UL,
        0xa05050f0UL, 0x783c3c44UL, 0x259f9fbaUL, 0x4ba8a8e3UL,
        0xa25151f3UL, 0x5da3a3feUL, 0x804040c0UL, 0x058f8f8aUL,
        0x3f9292adUL, 0x219d9dbcUL, 0x70383848UL, 0xf1f5f504UL,
        0x63bcbcdfUL, 0x77b6b6c1UL, 0xafdada75UL, 0x42212163UL,
        0x20101030UL, 0xe5ffff1aUL, 0xfdf3f30eUL, 0xbfd2d26dUL,
        0x81cdcd4cUL, 0x180c0c14UL, 0x26131335UL, 0xc3ecec2fUL,
        0xbe5f5fe1UL, 0x359797a2UL, 0x884444ccUL, 0x2e171739UL,
        0x93c4c457UL, 0x55a7a7f2UL, 0xfc7e7e82UL, 0x7a3d3d47UL,
        0xc86464acUL, 0xba5d5de7UL, 0x3219192bUL, 0xe6737395UL,
        0xc06060a0UL, 0x19818198UL, 0x9e4f4fd1UL, 0xa3dcdc7fUL,
        0x44222266UL, 0x542a2a7eUL, 0x3b9090abUL, 0x0b888883UL,
        0x8c4646caUL, 0xc7eeee29UL, 0x6bb8b8d3UL, 0x2814143cUL,
        0xa7dede79UL, 0xbc5e5ee2UL, 0x160b0b1dUL, 0xaddbdb76UL,
        0xdbe0e03bUL, 0x64323256UL, 0x743a3a4eUL, 0x140a0a1eUL,
        0x924949dbUL, 0x0c06060aUL, 0x4824246cUL, 0xb85c5ce4UL,
        0x9fc2c25dUL, 0xbdd3d36eUL, 0x43acacefUL, 0xc46262a6UL,
        0x399191a8UL, 0x319595a4UL, 0xd3e4e437UL, 0xf279798bUL,
        0xd5e7e732UL, 0x8bc8c843UL, 0x6e373759UL, 0xda6d6db7UL,
        0x018d8d8cUL, 0xb1d5d564UL, 0x9c4e4ed2UL, 0x49a9a9e0UL,
        0xd86c6cb4UL, 0xac5656faUL, 0xf3f4f407UL, 0xcfeaea25UL,
        0xca6565afUL, 0xf47a7a8eUL, 0x47aeaee9UL, 0x10080818UL,
        0x6fbabad5UL, 0xf0787888UL, 0x4a25256fUL, 0x5c2e2e72UL,
        0x381c1c24UL, 0x57a6a6f1UL, 0x73b4b4c7UL, 0x97c6c651UL,
        0xcbe8e823UL, 0xa1dddd7cUL, 0xe874749cUL, 0x3e1f1f21UL,
        0x964b4bddUL, 0x61bdbddcUL, 0x0d8b8b86UL, 0x0f8a8a85UL,
        0xe0707090UL, 0x7c3e3e42UL, 0x71b5b5c4UL, 0xcc6666aaUL,
        0x904848d8UL, 0x06030305UL, 0xf7f6f601UL, 0x1c0e0e12UL,
        0xc26161a3UL, 0x6a35355fUL, 0xae5757f9UL, 0x69b9b9d0UL,
        0x17868691UL, 0x99c1c158UL, 0x3a1d1d27UL, 0x279e9eb9UL,
        0xd9e1e138UL, 0xebf8f813UL, 0x2b9898b3UL, 0x22111133UL,
        0xd26969bbUL, 0xa9d9d970UL, 0x078e8e89UL, 0x339494a7UL,
        0x2d9b9bb6UL, 0x3c1e1e22UL, 0x15878792UL, 0xc9e9e920UL,
        0x87cece49UL, 0xaa5555ffUL, 0x50282878UL, 0xa5dfdf7aUL,
        0x038c8c8fUL, 0x59a1a1f8UL, 0x09898980UL, 0x1a0d0d17UL,
        0x65bfbfdaUL, 0xd7e6e631UL, 0x844242c6UL, 0xd06868b8UL,
        0x824141c3UL, 0x299999b0UL, 0x5a2d2d77UL, 0x1e0f0f11UL,
        0x7bb0b0cbUL, 0xa85454fcUL, 0x6dbbbbd6UL, 0x2c16163aUL,
    };

    static const u32 Te1[TableSize] =
    {
        0xa5c66363UL, 0x84f87c7cUL, 0x99ee7777UL, 0x8df67b7bUL,
        0x0dfff2f2UL, 0xbdd66b6bUL, 0xb1de6f6fUL, 0x5491c5c5UL,
        0x50603030UL, 0x03020101UL, 0xa9ce6767UL, 0x7d562b2bUL,
        0x19e7fefeUL, 0x62b5d7d7UL, 0xe64dababUL, 0x9aec7676UL,
        0x458fcacaUL, 0x9d1f8282UL, 0x4089c9c9UL, 0x87fa7d7dUL,
        0x15effafaUL, 0xebb25959UL, 0xc98e4747UL, 0x0bfbf0f0UL,
        0xec41adadUL, 0x67b3d4d4UL, 0xfd5fa2a2UL, 0xea45afafUL,
        0xbf239c9cUL, 0xf753a4a4UL, 0x96e47272UL, 0x5b9bc0c0UL,
        0xc275b7b7UL, 0x1ce1fdfdUL, 0xae3d9393UL, 0x6a4c2626UL,
        0x5a6c3636UL, 0x417e3f3fUL, 0x02f5f7f7UL, 0x4f83ccccUL,
        0x5c683434UL, 0xf451a5a5UL, 0x34d1e5e5UL, 0x08f9f1f1UL,
        0x93e27171UL, 0x73abd8d8UL, 0x53623131UL, 0x3f2a1515UL,
        0x0c080404UL, 0x5295c7c7UL, 0x65462323UL, 0x5e9dc3c3UL,
        0x28301818UL, 0xa1379696UL, 0x0f0a0505UL, 0xb52f9a9aUL,
        0x090e0707UL, 0x36241212UL, 0x9b1b8080UL, 0x3ddfe2e2UL,
        0x26cdebebUL, 0x694e2727UL, 0xcd7fb2b2UL, 0x9fea7575UL,
        0x1b120909UL, 0x9e1d8383UL, 0x74582c2cUL, 0x2e341a1aUL,
        0x2d361b1bUL, 0xb2dc6e6eUL, 0xeeb45a5aUL, 0xfb5ba0a0UL,
        0xf6a45252UL, 0x4d763b3bUL, 0x61b7d6d6UL, 0xce7db3b3UL,
        0x7b522929UL, 0x3edde3e3UL, 0x715e2f2fUL, 0x97138484UL,
        0xf5a65353UL, 0x68b9d1d1UL, 0x00000000UL, 0x2cc1ededUL,
        0x60402020UL, 0x1fe3fcfcUL, 0xc879b1b1UL, 0xedb65b5bUL,
        0xbed46a6aUL, 0x468dcbcbUL, 0xd967bebeUL, 0x4b723939UL,
        0xde944a4aUL, 0xd4984c4cUL, 0xe8b05858UL, 0x4a85cfcfUL,
        0x6bbbd0d0UL, 0x2ac5efefUL, 0xe54faaaaUL, 0x16edfbfbUL,
        0xc5864343UL, 0xd79a4d4dUL, 0x55663333UL, 0x94118585UL,
        0xcf8a4545UL, 0x10e9f9f9UL, 0x06040202UL, 0x81fe7f7fUL,
        0xf0a05050UL, 0x44783c3cUL, 0xba259f9fUL, 0xe34ba8a8UL,
        0xf3a25151UL, 0xfe5da3a3UL, 0xc0804040UL, 0x8a058f8fUL,
        0xad3f9292UL, 0xbc219d9dUL, 0x48703838UL, 0x04f1f5f5UL,
        0xdf63bcbcUL, 0xc177b6b6UL, 0x75afdadaUL, 0x63422121UL,
        0x30201010UL, 0x1ae5ffffUL, 0x0efdf3f3UL, 0x6dbfd2d2UL,
        0x4c81cdcdUL, 0x14180c0cUL, 0x35261313UL, 0x2fc3ececUL,
        0xe1be5f5fUL, 0xa2359797UL, 0xcc884444UL, 0x392e1717UL,
        0x5793c4c4UL, 0xf255a7a7UL, 0x82fc7e7eUL, 0x477a3d3dUL,
        0xacc86464UL, 0xe7ba5d5dUL, 0x2b321919UL, 0x95e67373UL,
        0xa0c06060UL, 0x98198181UL, 0xd19e4f4fUL, 0x7fa3dcdcUL,
        0x66442222UL, 0x7e542a2aUL, 0xab3b9090UL, 0x830b8888UL,
        0xca8c4646UL, 0x29c7eeeeUL, 0xd36bb8b8UL, 0x3c281414UL,
        0x79a7dedeUL, 0xe2bc5e5eUL, 0x1d160b0bUL, 0x76addbdbUL,
        0x3bdbe0e0UL, 0x56643232UL, 0x4e743a3aUL, 0x1e140a0aUL,
        0xdb924949UL, 0x0a0c0606UL, 0x6c482424UL, 0xe4b85c5cUL,
        0x5d9fc2c2UL, 0x6ebdd3d3UL, 0xef43acacUL, 0xa6c46262UL,
        0xa8399191UL, 0xa4319595UL, 0x37d3e4e4UL, 0x8bf27979UL,
        0x32d5e7e7UL, 0x438bc8c8UL, 0x596e3737UL, 0xb7da6d6dUL,
        0x8c018d8dUL, 0x64b1d5d5UL, 0xd29c4e4eUL, 0xe049a9a9UL,
        0xb4d86c6cUL, 0xfaac5656UL, 0x07f3f4f4UL, 0x25cfeaeaUL,
        0xafca6565UL, 0x8ef47a7aUL, 0xe947aeaeUL, 0x18100808UL,
        0xd56fbabaUL, 0x88f07878UL, 0x6f4a2525UL, 0x725c2e2eUL,
        0x24381c1cUL, 0xf157a6a6UL, 0xc773b4b4UL, 0x5197c6c6UL,
        0x23cbe8e8UL, 0x7ca1ddddUL, 0x9ce87474UL, 0x213e1f1fUL,
        0xdd964b4bUL, 0xdc61bdbdUL, 0x860d8b8bUL, 0x850f8a8aUL,
        0x90e07070UL, 0x427c3e3eUL, 0xc471b5b5UL, 0xaacc6666UL,
        0xd8904848UL, 0x05060303UL, 0x01f7f6f6UL, 0x121c0e0eUL,
        0xa3c26161UL, 0x5f6a3535UL, 0xf9ae5757UL, 0xd069b9b9UL,
        0x91178686UL, 0x5899c1c1UL, 0x273a1d1dUL, 0xb9279e9eUL,
        0x38d9e1e1UL, 0x13ebf8f8UL, 0xb32b9898UL, 0x33221111UL,
        0xbbd26969UL, 0x70a9d9d9UL, 0x89078e8eUL, 0xa7339494UL,
        0xb62d9b9bUL, 0x223c1e1eUL, 0x92158787UL, 0x20c9e9e9UL,
        0x4987ceceUL, 0xffaa5555UL, 0x78502828UL, 0x7aa5dfdfUL,
        0x8f038c8cUL, 0xf859a1a1UL, 0x80098989UL, 0x171a0d0dUL,
        0xda65bfbfUL, 0x31d7e6e6UL, 0xc6844242UL, 0xb8d06868UL,
        0xc3824141UL, 0xb0299999UL, 0x775a2d2dUL, 0x111e0f0fUL,
        0xcb7bb0b0UL, 0xfca85454UL, 0xd66dbbbbUL, 0x3a2c1616UL,
    };
    static const u32 Te2[TableSize] =
    {
        0x63a5c663UL, 0x7c84f87cUL, 0x7799ee77UL, 0x7b8df67bUL,
        0xf20dfff2UL, 0x6bbdd66bUL, 0x6fb1de6fUL, 0xc55491c5UL,
        0x30506030UL, 0x01030201UL, 0x67a9ce67UL, 0x2b7d562bUL,
        0xfe19e7feUL, 0xd762b5d7UL, 0xabe64dabUL, 0x769aec76UL,
        0xca458fcaUL, 0x829d1f82UL, 0xc94089c9UL, 0x7d87fa7dUL,
        0xfa15effaUL, 0x59ebb259UL, 0x47c98e47UL, 0xf00bfbf0UL,
        0xadec41adUL, 0xd467b3d4UL, 0xa2fd5fa2UL, 0xafea45afUL,
        0x9cbf239cUL, 0xa4f753a4UL, 0x7296e472UL, 0xc05b9bc0UL,
        0xb7c275b7UL, 0xfd1ce1fdUL, 0x93ae3d93UL, 0x266a4c26UL,
        0x365a6c36UL, 0x3f417e3fUL, 0xf702f5f7UL, 0xcc4f83ccUL,
        0x345c6834UL, 0xa5f451a5UL, 0xe534d1e5UL, 0xf108f9f1UL,
        0x7193e271UL, 0xd873abd8UL, 0x31536231UL, 0x153f2a15UL,
        0x040c0804UL, 0xc75295c7UL, 0x23654623UL, 0xc35e9dc3UL,
        0x18283018UL, 0x96a13796UL, 0x050f0a05UL, 0x9ab52f9aUL,
        0x07090e07UL, 0x12362412UL, 0x809b1b80UL, 0xe23ddfe2UL,
        0xeb26cdebUL, 0x27694e27UL, 0xb2cd7fb2UL, 0x759fea75UL,
        0x091b1209UL, 0x839e1d83UL, 0x2c74582cUL, 0x1a2e341aUL,
        0x1b2d361bUL, 0x6eb2dc6eUL, 0x5aeeb45aUL, 0xa0fb5ba0UL,
        0x52f6a452UL, 0x3b4d763bUL, 0xd661b7d6UL, 0xb3ce7db3UL,
        0x297b5229UL, 0xe33edde3UL, 0x2f715e2fUL, 0x84971384UL,
        0x53f5a653UL, 0xd168b9d1UL, 0x00000000UL, 0xed2cc1edUL,
        0x20604020UL, 0xfc1fe3fcUL, 0xb1c879b1UL, 0x5bedb65bUL,
        0x6abed46aUL, 0xcb468dcbUL, 0xbed967beUL, 0x394b7239UL,
        0x4ade944aUL, 0x4cd4984cUL, 0x58e8b058UL, 0xcf4a85cfUL,
        0xd06bbbd0UL, 0xef2ac5efUL, 0xaae54faaUL, 0xfb16edfbUL,
        0x43c58643UL, 0x4dd79a4dUL, 0x33556633UL, 0x85941185UL,
        0x45cf8a45UL, 0xf910e9f9UL, 0x02060402UL, 0x7f81fe7fUL,
        0x50f0a050UL, 0x3c44783cUL, 0x9fba259fUL, 0xa8e34ba8UL,
        0x51f3a251UL, 0xa3fe5da3UL, 0x40c08040UL, 0x8f8a058fUL,
        0x92ad3f92UL, 0x9dbc219dUL, 0x38487038UL, 0xf504f1f5UL,
        0xbcdf63bcUL, 0xb6c177b6UL, 0xda75afdaUL, 0x21634221UL,
        0x10302010UL, 0xff1ae5ffUL, 0xf30efdf3UL, 0xd26dbfd2UL,
        0xcd4c81cdUL, 0x0c14180cUL, 0x13352613UL, 0xec2fc3ecUL,
        0x5fe1be5fUL, 0x97a23597UL, 0x44cc8844UL, 0x17392e17UL,
        0xc45793c4UL, 0xa7f255a7UL, 0x7e82fc7eUL, 0x3d477a3dUL,
        0x64acc864UL, 0x5de7ba5dUL, 0x192b3219UL, 0x7395e673UL,
        0x60a0c060UL, 0x81981981UL, 0x4fd19e4fUL, 0xdc7fa3dcUL,
        0x22664422UL, 0x2a7e542aUL, 0x90ab3b90UL, 0x88830b88UL,
        0x46ca8c46UL, 0xee29c7eeUL, 0xb8d36bb8UL, 0x143c2814UL,
        0xde79a7deUL, 0x5ee2bc5eUL, 0x0b1d160bUL, 0xdb76addbUL,
        0xe03bdbe0UL, 0x32566432UL, 0x3a4e743aUL, 0x0a1e140aUL,
        0x49db9249UL, 0x060a0c06UL, 0x246c4824UL, 0x5ce4b85cUL,
        0xc25d9fc2UL, 0xd36ebdd3UL, 0xacef43acUL, 0x62a6c462UL,
        0x91a83991UL, 0x95a43195UL, 0xe437d3e4UL, 0x798bf279UL,
        0xe732d5e7UL, 0xc8438bc8UL, 0x37596e37UL, 0x6db7da6dUL,
        0x8d8c018dUL, 0xd564b1d5UL, 0x4ed29c4eUL, 0xa9e049a9UL,
        0x6cb4d86cUL, 0x56faac56UL, 0xf407f3f4UL, 0xea25cfeaUL,
        0x65afca65UL, 0x7a8ef47aUL, 0xaee947aeUL, 0x08181008UL,
        0xbad56fbaUL, 0x7888f078UL, 0x256f4a25UL, 0x2e725c2eUL,
        0x1c24381cUL, 0xa6f157a6UL, 0xb4c773b4UL, 0xc65197c6UL,
        0xe823cbe8UL, 0xdd7ca1ddUL, 0x749ce874UL, 0x1f213e1fUL,
        0x4bdd964bUL, 0xbddc61bdUL, 0x8b860d8bUL, 0x8a850f8aUL,
        0x7090e070UL, 0x3e427c3eUL, 0xb5c471b5UL, 0x66aacc66UL,
        0x48d89048UL, 0x03050603UL, 0xf601f7f6UL, 0x0e121c0eUL,
        0x61a3c261UL, 0x355f6a35UL, 0x57f9ae57UL, 0xb9d069b9UL,
        0x86911786UL, 0xc15899c1UL, 0x1d273a1dUL, 0x9eb9279eUL,
        0xe138d9e1UL, 0xf813ebf8UL, 0x98b32b98UL, 0x11332211UL,
        0x69bbd269UL, 0xd970a9d9UL, 0x8e89078eUL, 0x94a73394UL,
        0x9bb62d9bUL, 0x1e223c1eUL, 0x87921587UL, 0xe920c9e9UL,
        0xce4987ceUL, 0x55ffaa55UL, 0x28785028UL, 0xdf7aa5dfUL,
        0x8c8f038cUL, 0xa1f859a1UL, 0x89800989UL, 0x0d171a0dUL,
        0xbfda65bfUL, 0xe631d7e6UL, 0x42c68442UL, 0x68b8d068UL,
        0x41c38241UL, 0x99b02999UL, 0x2d775a2dUL, 0x0f111e0fUL,
        0xb0cb7bb0UL, 0x54fca854UL, 0xbbd66dbbUL, 0x163a2c16UL,
    };
    static const u32 Te3[TableSize] =
    {
        0x6363a5c6UL, 0x7c7c84f8UL, 0x777799eeUL, 0x7b7b8df6UL,
        0xf2f20dffUL, 0x6b6bbdd6UL, 0x6f6fb1deUL, 0xc5c55491UL,
        0x30305060UL, 0x01010302UL, 0x6767a9ceUL, 0x2b2b7d56UL,
        0xfefe19e7UL, 0xd7d762b5UL, 0xababe64dUL, 0x76769aecUL,
        0xcaca458fUL, 0x82829d1fUL, 0xc9c94089UL, 0x7d7d87faUL,
        0xfafa15efUL, 0x5959ebb2UL, 0x4747c98eUL, 0xf0f00bfbUL,
        0xadadec41UL, 0xd4d467b3UL, 0xa2a2fd5fUL, 0xafafea45UL,
        0x9c9cbf23UL, 0xa4a4f753UL, 0x727296e4UL, 0xc0c05b9bUL,
        0xb7b7c275UL, 0xfdfd1ce1UL, 0x9393ae3dUL, 0x26266a4cUL,
        0x36365a6cUL, 0x3f3f417eUL, 0xf7f702f5UL, 0xcccc4f83UL,
        0x34345c68UL, 0xa5a5f451UL, 0xe5e534d1UL, 0xf1f108f9UL,
        0x717193e2UL, 0xd8d873abUL, 0x31315362UL, 0x15153f2aUL,
        0x04040c08UL, 0xc7c75295UL, 0x23236546UL, 0xc3c35e9dUL,
        0x18182830UL, 0x9696a137UL, 0x05050f0aUL, 0x9a9ab52fUL,
        0x0707090eUL, 0x12123624UL, 0x80809b1bUL, 0xe2e23ddfUL,
        0xebeb26cdUL, 0x2727694eUL, 0xb2b2cd7fUL, 0x75759feaUL,
        0x09091b12UL, 0x83839e1dUL, 0x2c2c7458UL, 0x1a1a2e34UL,
        0x1b1b2d36UL, 0x6e6eb2dcUL, 0x5a5aeeb4UL, 0xa0a0fb5bUL,
        0x5252f6a4UL, 0x3b3b4d76UL, 0xd6d661b7UL, 0xb3b3ce7dUL,
        0x29297b52UL, 0xe3e33eddUL, 0x2f2f715eUL, 0x84849713UL,
        0x5353f5a6UL, 0xd1d168b9UL, 0x00000000UL, 0xeded2cc1UL,
        0x20206040UL, 0xfcfc1fe3UL, 0xb1b1c879UL, 0x5b5bedb6UL,
        0x6a6abed4UL, 0xcbcb468dUL, 0xbebed967UL, 0x39394b72UL,
        0x4a4ade94UL, 0x4c4cd498UL, 0x5858e8b0UL, 0xcfcf4a85UL,
        0xd0d06bbbUL, 0xefef2ac5UL, 0xaaaae54fUL, 0xfbfb16edUL,
        0x4343c586UL, 0x4d4dd79aUL, 0x33335566UL, 0x85859411UL,
        0x4545cf8aUL, 0xf9f910e9UL, 0x02020604UL, 0x7f7f81feUL,
        0x5050f0a0UL, 0x3c3c4478UL, 0x9f9fba25UL, 0xa8a8e34bUL,
        0x5151f3a2UL, 0xa3a3fe5dUL, 0x4040c080UL, 0x8f8f8a05UL,
        0x9292ad3fUL, 0x9d9dbc21UL, 0x38384870UL, 0xf5f504f1UL,
        0xbcbcdf63UL, 0xb6b6c177UL, 0xdada75afUL, 0x21216342UL,
        0x10103020UL, 0xffff1ae5UL, 0xf3f30efdUL, 0xd2d26dbfUL,
        0xcdcd4c81UL, 0x0c0c1418UL, 0x13133526UL, 0xecec2fc3UL,
        0x5f5fe1beUL, 0x9797a235UL, 0x4444cc88UL, 0x1717392eUL,
        0xc4c45793UL, 0xa7a7f255UL, 0x7e7e82fcUL, 0x3d3d477aUL,
        0x6464acc8UL, 0x5d5de7baUL, 0x19192b32UL, 0x737395e6UL,
        0x6060a0c0UL, 0x81819819UL, 0x4f4fd19eUL, 0xdcdc7fa3UL,
        0x22226644UL, 0x2a2a7e54UL, 0x9090ab3bUL, 0x8888830bUL,
        0x4646ca8cUL, 0xeeee29c7UL, 0xb8b8d36bUL, 0x14143c28UL,
        0xdede79a7UL, 0x5e5ee2bcUL, 0x0b0b1d16UL, 0xdbdb76adUL,
        0xe0e03bdbUL, 0x32325664UL, 0x3a3a4e74UL, 0x0a0a1e14UL,
        0x4949db92UL, 0x06060a0cUL, 0x24246c48UL, 0x5c5ce4b8UL,
        0xc2c25d9fUL, 0xd3d36ebdUL, 0xacacef43UL, 0x6262a6c4UL,
        0x9191a839UL, 0x9595a431UL, 0xe4e437d3UL, 0x79798bf2UL,
        0xe7e732d5UL, 0xc8c8438bUL, 0x3737596eUL, 0x6d6db7daUL,
        0x8d8d8c01UL, 0xd5d564b1UL, 0x4e4ed29cUL, 0xa9a9e049UL,
        0x6c6cb4d8UL, 0x5656faacUL, 0xf4f407f3UL, 0xeaea25cfUL,
        0x6565afcaUL, 0x7a7a8ef4UL, 0xaeaee947UL, 0x08081810UL,
        0xbabad56fUL, 0x787888f0UL, 0x25256f4aUL, 0x2e2e725cUL,
        0x1c1c2438UL, 0xa6a6f157UL, 0xb4b4c773UL, 0xc6c65197UL,
        0xe8e823cbUL, 0xdddd7ca1UL, 0x74749ce8UL, 0x1f1f213eUL,
        0x4b4bdd96UL, 0xbdbddc61UL, 0x8b8b860dUL, 0x8a8a850fUL,
        0x707090e0UL, 0x3e3e427cUL, 0xb5b5c471UL, 0x6666aaccUL,
        0x4848d890UL, 0x03030506UL, 0xf6f601f7UL, 0x0e0e121cUL,
        0x6161a3c2UL, 0x35355f6aUL, 0x5757f9aeUL, 0xb9b9d069UL,
        0x86869117UL, 0xc1c15899UL, 0x1d1d273aUL, 0x9e9eb927UL,
        0xe1e138d9UL, 0xf8f813ebUL, 0x9898b32bUL, 0x11113322UL,
        0x6969bbd2UL, 0xd9d970a9UL, 0x8e8e8907UL, 0x9494a733UL,
        0x9b9bb62dUL, 0x1e1e223cUL, 0x87879215UL, 0xe9e920c9UL,
        0xcece4987UL, 0x5555ffaaUL, 0x28287850UL, 0xdfdf7aa5UL,
        0x8c8c8f03UL, 0xa1a1f859UL, 0x89898009UL, 0x0d0d171aUL,
        0xbfbfda65UL, 0xe6e631d7UL, 0x4242c684UL, 0x6868b8d0UL,
        0x4141c382UL, 0x9999b029UL, 0x2d2d775aUL, 0x0f0f111eUL,
        0xb0b0cb7bUL, 0x5454fca8UL, 0xbbbbd66dUL, 0x16163a2cUL,
    };

    static const u32 Te4[TableSize] =
    {
        0x63636363UL, 0x7c7c7c7cUL, 0x77777777UL, 0x7b7b7b7bUL,
        0xf2f2f2f2UL, 0x6b6b6b6bUL, 0x6f6f6f6fUL, 0xc5c5c5c5UL,
        0x30303030UL, 0x01010101UL, 0x67676767UL, 0x2b2b2b2bUL,
        0xfefefefeUL, 0xd7d7d7d7UL, 0xababababUL, 0x76767676UL,
        0xcacacacaUL, 0x82828282UL, 0xc9c9c9c9UL, 0x7d7d7d7dUL,
        0xfafafafaUL, 0x59595959UL, 0x47474747UL, 0xf0f0f0f0UL,
        0xadadadadUL, 0xd4d4d4d4UL, 0xa2a2a2a2UL, 0xafafafafUL,
        0x9c9c9c9cUL, 0xa4a4a4a4UL, 0x72727272UL, 0xc0c0c0c0UL,
        0xb7b7b7b7UL, 0xfdfdfdfdUL, 0x93939393UL, 0x26262626UL,
        0x36363636UL, 0x3f3f3f3fUL, 0xf7f7f7f7UL, 0xccccccccUL,
        0x34343434UL, 0xa5a5a5a5UL, 0xe5e5e5e5UL, 0xf1f1f1f1UL,
        0x71717171UL, 0xd8d8d8d8UL, 0x31313131UL, 0x15151515UL,
        0x04040404UL, 0xc7c7c7c7UL, 0x23232323UL, 0xc3c3c3c3UL,
        0x18181818UL, 0x96969696UL, 0x05050505UL, 0x9a9a9a9aUL,
        0x07070707UL, 0x12121212UL, 0x80808080UL, 0xe2e2e2e2UL,
        0xebebebebUL, 0x27272727UL, 0xb2b2b2b2UL, 0x75757575UL,
        0x09090909UL, 0x83838383UL, 0x2c2c2c2cUL, 0x1a1a1a1aUL,
        0x1b1b1b1bUL, 0x6e6e6e6eUL, 0x5a5a5a5aUL, 0xa0a0a0a0UL,
        0x52525252UL, 0x3b3b3b3bUL, 0xd6d6d6d6UL, 0xb3b3b3b3UL,
        0x29292929UL, 0xe3e3e3e3UL, 0x2f2f2f2fUL, 0x84848484UL,
        0x53535353UL, 0xd1d1d1d1UL, 0x00000000UL, 0xededededUL,
        0x20202020UL, 0xfcfcfcfcUL, 0xb1b1b1b1UL, 0x5b5b5b5bUL,
        0x6a6a6a6aUL, 0xcbcbcbcbUL, 0xbebebebeUL, 0x39393939UL,
        0x4a4a4a4aUL, 0x4c4c4c4cUL, 0x58585858UL, 0xcfcfcfcfUL,
        0xd0d0d0d0UL, 0xefefefefUL, 0xaaaaaaaaUL, 0xfbfbfbfbUL,
        0x43434343UL, 0x4d4d4d4dUL, 0x33333333UL, 0x85858585UL,
        0x45454545UL, 0xf9f9f9f9UL, 0x02020202UL, 0x7f7f7f7fUL,
        0x50505050UL, 0x3c3c3c3cUL, 0x9f9f9f9fUL, 0xa8a8a8a8UL,
        0x51515151UL, 0xa3a3a3a3UL, 0x40404040UL, 0x8f8f8f8fUL,
        0x92929292UL, 0x9d9d9d9dUL, 0x38383838UL, 0xf5f5f5f5UL,
        0xbcbcbcbcUL, 0xb6b6b6b6UL, 0xdadadadaUL, 0x21212121UL,
        0x10101010UL, 0xffffffffUL, 0xf3f3f3f3UL, 0xd2d2d2d2UL,
        0xcdcdcdcdUL, 0x0c0c0c0cUL, 0x13131313UL, 0xececececUL,
        0x5f5f5f5fUL, 0x97979797UL, 0x44444444UL, 0x17171717UL,
        0xc4c4c4c4UL, 0xa7a7a7a7UL, 0x7e7e7e7eUL, 0x3d3d3d3dUL,
        0x64646464UL, 0x5d5d5d5dUL, 0x19191919UL, 0x73737373UL,
        0x60606060UL, 0x81818181UL, 0x4f4f4f4fUL, 0xdcdcdcdcUL,
        0x22222222UL, 0x2a2a2a2aUL, 0x90909090UL, 0x88888888UL,
        0x46464646UL, 0xeeeeeeeeUL, 0xb8b8b8b8UL, 0x14141414UL,
        0xdedededeUL, 0x5e5e5e5eUL, 0x0b0b0b0bUL, 0xdbdbdbdbUL,
        0xe0e0e0e0UL, 0x32323232UL, 0x3a3a3a3aUL, 0x0a0a0a0aUL,
        0x49494949UL, 0x06060606UL, 0x24242424UL, 0x5c5c5c5cUL,
        0xc2c2c2c2UL, 0xd3d3d3d3UL, 0xacacacacUL, 0x62626262UL,
        0x91919191UL, 0x95959595UL, 0xe4e4e4e4UL, 0x79797979UL,
        0xe7e7e7e7UL, 0xc8c8c8c8UL, 0x37373737UL, 0x6d6d6d6dUL,
        0x8d8d8d8dUL, 0xd5d5d5d5UL, 0x4e4e4e4eUL, 0xa9a9a9a9UL,
        0x6c6c6c6cUL, 0x56565656UL, 0xf4f4f4f4UL, 0xeaeaeaeaUL,
        0x65656565UL, 0x7a7a7a7aUL, 0xaeaeaeaeUL, 0x08080808UL,
        0xbabababaUL, 0x78787878UL, 0x25252525UL, 0x2e2e2e2eUL,
        0x1c1c1c1cUL, 0xa6a6a6a6UL, 0xb4b4b4b4UL, 0xc6c6c6c6UL,
        0xe8e8e8e8UL, 0xddddddddUL, 0x74747474UL, 0x1f1f1f1fUL,
        0x4b4b4b4bUL, 0xbdbdbdbdUL, 0x8b8b8b8bUL, 0x8a8a8a8aUL,
        0x70707070UL, 0x3e3e3e3eUL, 0xb5b5b5b5UL, 0x66666666UL,
        0x48484848UL, 0x03030303UL, 0xf6f6f6f6UL, 0x0e0e0e0eUL,
        0x61616161UL, 0x35353535UL, 0x57575757UL, 0xb9b9b9b9UL,
        0x86868686UL, 0xc1c1c1c1UL, 0x1d1d1d1dUL, 0x9e9e9e9eUL,
        0xe1e1e1e1UL, 0xf8f8f8f8UL, 0x98989898UL, 0x11111111UL,
        0x69696969UL, 0xd9d9d9d9UL, 0x8e8e8e8eUL, 0x94949494UL,
        0x9b9b9b9bUL, 0x1e1e1e1eUL, 0x87878787UL, 0xe9e9e9e9UL,
        0xcecececeUL, 0x55555555UL, 0x28282828UL, 0xdfdfdfdfUL,
        0x8c8c8c8cUL, 0xa1a1a1a1UL, 0x89898989UL, 0x0d0d0d0dUL,
        0xbfbfbfbfUL, 0xe6e6e6e6UL, 0x42424242UL, 0x68686868UL,
        0x41414141UL, 0x99999999UL, 0x2d2d2d2dUL, 0x0f0f0f0fUL,
        0xb0b0b0b0UL, 0x54545454UL, 0xbbbbbbbbUL, 0x16161616UL,
    };

    static const u32 Td0[TableSize] =
    {
        0x51f4a750UL, 0x7e416553UL, 0x1a17a4c3UL, 0x3a275e96UL,
        0x3bab6bcbUL, 0x1f9d45f1UL, 0xacfa58abUL, 0x4be30393UL,
        0x2030fa55UL, 0xad766df6UL, 0x88cc7691UL, 0xf5024c25UL,
        0x4fe5d7fcUL, 0xc52acbd7UL, 0x26354480UL, 0xb562a38fUL,
        0xdeb15a49UL, 0x25ba1b67UL, 0x45ea0e98UL, 0x5dfec0e1UL,
        0xc32f7502UL, 0x814cf012UL, 0x8d4697a3UL, 0x6bd3f9c6UL,
        0x038f5fe7UL, 0x15929c95UL, 0xbf6d7aebUL, 0x955259daUL,
        0xd4be832dUL, 0x587421d3UL, 0x49e06929UL, 0x8ec9c844UL,
        0x75c2896aUL, 0xf48e7978UL, 0x99583e6bUL, 0x27b971ddUL,
        0xbee14fb6UL, 0xf088ad17UL, 0xc920ac66UL, 0x7dce3ab4UL,
        0x63df4a18UL, 0xe51a3182UL, 0x97513360UL, 0x62537f45UL,
        0xb16477e0UL, 0xbb6bae84UL, 0xfe81a01cUL, 0xf9082b94UL,
        0x70486858UL, 0x8f45fd19UL, 0x94de6c87UL, 0x527bf8b7UL,
        0xab73d323UL, 0x724b02e2UL, 0xe31f8f57UL, 0x6655ab2aUL,
        0xb2eb2807UL, 0x2fb5c203UL, 0x86c57b9aUL, 0xd33708a5UL,
        0x302887f2UL, 0x23bfa5b2UL, 0x02036abaUL, 0xed16825cUL,
        0x8acf1c2bUL, 0xa779b492UL, 0xf307f2f0UL, 0x4e69e2a1UL,
        0x65daf4cdUL, 0x0605bed5UL, 0xd134621fUL, 0xc4a6fe8aUL,
        0x342e539dUL, 0xa2f355a0UL, 0x058ae132UL, 0xa4f6eb75UL,
        0x0b83ec39UL, 0x4060efaaUL, 0x5e719f06UL, 0xbd6e1051UL,
        0x3e218af9UL, 0x96dd063dUL, 0xdd3e05aeUL, 0x4de6bd46UL,
        0x91548db5UL, 0x71c45d05UL, 0x0406d46fUL, 0x605015ffUL,
        0x1998fb24UL, 0xd6bde997UL, 0x894043ccUL, 0x67d99e77UL,
        0xb0e842bdUL, 0x07898b88UL, 0xe7195b38UL, 0x79c8eedbUL,
        0xa17c0a47UL, 0x7c420fe9UL, 0xf8841ec9UL, 0x00000000UL,
        0x09808683UL, 0x322bed48UL, 0x1e1170acUL, 0x6c5a724eUL,
        0xfd0efffbUL, 0x0f853856UL, 0x3daed51eUL, 0x362d3927UL,
        0x0a0fd964UL, 0x685ca621UL, 0x9b5b54d1UL, 0x24362e3aUL,
        0x0c0a67b1UL, 0x9357e70fUL, 0xb4ee96d2UL, 0x1b9b919eUL,
        0x80c0c54fUL, 0x61dc20a2UL, 0x5a774b69UL, 0x1c121a16UL,
        0xe293ba0aUL, 0xc0a02ae5UL, 0x3c22e043UL, 0x121b171dUL,
        0x0e090d0bUL, 0xf28bc7adUL, 0x2db6a8b9UL, 0x141ea9c8UL,
        0x57f11985UL, 0xaf75074cUL, 0xee99ddbbUL, 0xa37f60fdUL,
        0xf701269fUL, 0x5c72f5bcUL, 0x44663bc5UL, 0x5bfb7e34UL,
        0x8b432976UL, 0xcb23c6dcUL, 0xb6edfc68UL, 0xb8e4f163UL,
        0xd731dccaUL, 0x42638510UL, 0x13972240UL, 0x84c61120UL,
        0x854a247dUL, 0xd2bb3df8UL, 0xaef93211UL, 0xc729a16dUL,
        0x1d9e2f4bUL, 0xdcb230f3UL, 0x0d8652ecUL, 0x77c1e3d0UL,
        0x2bb3166cUL, 0xa970b999UL, 0x119448faUL, 0x47e96422UL,
        0xa8fc8cc4UL, 0xa0f03f1aUL, 0x567d2cd8UL, 0x223390efUL,
        0x87494ec7UL, 0xd938d1c1UL, 0x8ccaa2feUL, 0x98d40b36UL,
        0xa6f581cfUL, 0xa57ade28UL, 0xdab78e26UL, 0x3fadbfa4UL,
        0x2c3a9de4UL, 0x5078920dUL, 0x6a5fcc9bUL, 0x547e4662UL,
        0xf68d13c2UL, 0x90d8b8e8UL, 0x2e39f75eUL, 0x82c3aff5UL,
        0x9f5d80beUL, 0x69d0937cUL, 0x6fd52da9UL, 0xcf2512b3UL,
        0xc8ac993bUL, 0x10187da7UL, 0xe89c636eUL, 0xdb3bbb7bUL,
        0xcd267809UL, 0x6e5918f4UL, 0xec9ab701UL, 0x834f9aa8UL,
        0xe6956e65UL, 0xaaffe67eUL, 0x21bccf08UL, 0xef15e8e6UL,
        0xbae79bd9UL, 0x4a6f36ceUL, 0xea9f09d4UL, 0x29b07cd6UL,
        0x31a4b2afUL, 0x2a3f2331UL, 0xc6a59430UL, 0x35a266c0UL,
        0x744ebc37UL, 0xfc82caa6UL, 0xe090d0b0UL, 0x33a7d815UL,
        0xf104984aUL, 0x41ecdaf7UL, 0x7fcd500eUL, 0x1791f62fUL,
        0x764dd68dUL, 0x43efb04dUL, 0xccaa4d54UL, 0xe49604dfUL,
        0x9ed1b5e3UL, 0x4c6a881bUL, 0xc12c1fb8UL, 0x4665517fUL,
        0x9d5eea04UL, 0x018c355dUL, 0xfa877473UL, 0xfb0b412eUL,
        0xb3671d5aUL, 0x92dbd252UL, 0xe9105633UL, 0x6dd64713UL,
        0x9ad7618cUL, 0x37a10c7aUL, 0x59f8148eUL, 0xeb133c89UL,
        0xcea927eeUL, 0xb761c935UL, 0xe11ce5edUL, 0x7a47b13cUL,
        0x9cd2df59UL, 0x55f2733fUL, 0x1814ce79UL, 0x73c737bfUL,
        0x53f7cdeaUL, 0x5ffdaa5bUL, 0xdf3d6f14UL, 0x7844db86UL,
        0xcaaff381UL, 0xb968c43eUL, 0x3824342cUL, 0xc2a3405fUL,
        0x161dc372UL, 0xbce2250cUL, 0x283c498bUL, 0xff0d9541UL,
        0x39a80171UL, 0x080cb3deUL, 0xd8b4e49cUL, 0x6456c190UL,
        0x7bcb8461UL, 0xd532b670UL, 0x486c5c74UL, 0xd0b85742UL,
    };

    static const u32 Td1[TableSize] =
    {
        0x5051f4a7UL, 0x537e4165UL, 0xc31a17a4UL, 0x963a275eUL,
        0xcb3bab6bUL, 0xf11f9d45UL, 0xabacfa58UL, 0x934be303UL,
        0x552030faUL, 0xf6ad766dUL, 0x9188cc76UL, 0x25f5024cUL,
        0xfc4fe5d7UL, 0xd7c52acbUL, 0x80263544UL, 0x8fb562a3UL,
        0x49deb15aUL, 0x6725ba1bUL, 0x9845ea0eUL, 0xe15dfec0UL,
        0x02c32f75UL, 0x12814cf0UL, 0xa38d4697UL, 0xc66bd3f9UL,
        0xe7038f5fUL, 0x9515929cUL, 0xebbf6d7aUL, 0xda955259UL,
        0x2dd4be83UL, 0xd3587421UL, 0x2949e069UL, 0x448ec9c8UL,
        0x6a75c289UL, 0x78f48e79UL, 0x6b99583eUL, 0xdd27b971UL,
        0xb6bee14fUL, 0x17f088adUL, 0x66c920acUL, 0xb47dce3aUL,
        0x1863df4aUL, 0x82e51a31UL, 0x60975133UL, 0x4562537fUL,
        0xe0b16477UL, 0x84bb6baeUL, 0x1cfe81a0UL, 0x94f9082bUL,
        0x58704868UL, 0x198f45fdUL, 0x8794de6cUL, 0xb7527bf8UL,
        0x23ab73d3UL, 0xe2724b02UL, 0x57e31f8fUL, 0x2a6655abUL,
        0x07b2eb28UL, 0x032fb5c2UL, 0x9a86c57bUL, 0xa5d33708UL,
        0xf2302887UL, 0xb223bfa5UL, 0xba02036aUL, 0x5ced1682UL,
        0x2b8acf1cUL, 0x92a779b4UL, 0xf0f307f2UL, 0xa14e69e2UL,
        0xcd65daf4UL, 0xd50605beUL, 0x1fd13462UL, 0x8ac4a6feUL,
        0x9d342e53UL, 0xa0a2f355UL, 0x32058ae1UL, 0x75a4f6ebUL,
        0x390b83ecUL, 0xaa4060efUL, 0x065e719fUL, 0x51bd6e10UL,
        0xf93e218aUL, 0x3d96dd06UL, 0xaedd3e05UL, 0x464de6bdUL,
        0xb591548dUL, 0x0571c45dUL, 0x6f0406d4UL, 0xff605015UL,
        0x241998fbUL, 0x97d6bde9UL, 0xcc894043UL, 0x7767d99eUL,
        0xbdb0e842UL, 0x8807898bUL, 0x38e7195bUL, 0xdb79c8eeUL,
        0x47a17c0aUL, 0xe97c420fUL, 0xc9f8841eUL, 0x00000000UL,
        0x83098086UL, 0x48322bedUL, 0xac1e1170UL, 0x4e6c5a72UL,
        0xfbfd0effUL, 0x560f8538UL, 0x1e3daed5UL, 0x27362d39UL,
        0x640a0fd9UL, 0x21685ca6UL, 0xd19b5b54UL, 0x3a24362eUL,
        0xb10c0a67UL, 0x0f9357e7UL, 0xd2b4ee96UL, 0x9e1b9b91UL,
        0x4f80c0c5UL, 0xa261dc20UL, 0x695a774bUL, 0x161c121aUL,
        0x0ae293baUL, 0xe5c0a02aUL, 0x433c22e0UL, 0x1d121b17UL,
        0x0b0e090dUL, 0xadf28bc7UL, 0xb92db6a8UL, 0xc8141ea9UL,
        0x8557f119UL, 0x4caf7507UL, 0xbbee99ddUL, 0xfda37f60UL,
        0x9ff70126UL, 0xbc5c72f5UL, 0xc544663bUL, 0x345bfb7eUL,
        0x768b4329UL, 0xdccb23c6UL, 0x68b6edfcUL, 0x63b8e4f1UL,
        0xcad731dcUL, 0x10426385UL, 0x40139722UL, 0x2084c611UL,
        0x7d854a24UL, 0xf8d2bb3dUL, 0x11aef932UL, 0x6dc729a1UL,
        0x4b1d9e2fUL, 0xf3dcb230UL, 0xec0d8652UL, 0xd077c1e3UL,
        0x6c2bb316UL, 0x99a970b9UL, 0xfa119448UL, 0x2247e964UL,
        0xc4a8fc8cUL, 0x1aa0f03fUL, 0xd8567d2cUL, 0xef223390UL,
        0xc787494eUL, 0xc1d938d1UL, 0xfe8ccaa2UL, 0x3698d40bUL,
        0xcfa6f581UL, 0x28a57adeUL, 0x26dab78eUL, 0xa43fadbfUL,
        0xe42c3a9dUL, 0x0d507892UL, 0x9b6a5fccUL, 0x62547e46UL,
        0xc2f68d13UL, 0xe890d8b8UL, 0x5e2e39f7UL, 0xf582c3afUL,
        0xbe9f5d80UL, 0x7c69d093UL, 0xa96fd52dUL, 0xb3cf2512UL,
        0x3bc8ac99UL, 0xa710187dUL, 0x6ee89c63UL, 0x7bdb3bbbUL,
        0x09cd2678UL, 0xf46e5918UL, 0x01ec9ab7UL, 0xa8834f9aUL,
        0x65e6956eUL, 0x7eaaffe6UL, 0x0821bccfUL, 0xe6ef15e8UL,
        0xd9bae79bUL, 0xce4a6f36UL, 0xd4ea9f09UL, 0xd629b07cUL,
        0xaf31a4b2UL, 0x312a3f23UL, 0x30c6a594UL, 0xc035a266UL,
        0x37744ebcUL, 0xa6fc82caUL, 0xb0e090d0UL, 0x1533a7d8UL,
        0x4af10498UL, 0xf741ecdaUL, 0x0e7fcd50UL, 0x2f1791f6UL,
        0x8d764dd6UL, 0x4d43efb0UL, 0x54ccaa4dUL, 0xdfe49604UL,
        0xe39ed1b5UL, 0x1b4c6a88UL, 0xb8c12c1fUL, 0x7f466551UL,
        0x049d5eeaUL, 0x5d018c35UL, 0x73fa8774UL, 0x2efb0b41UL,
        0x5ab3671dUL, 0x5292dbd2UL, 0x33e91056UL, 0x136dd647UL,
        0x8c9ad761UL, 0x7a37a10cUL, 0x8e59f814UL, 0x89eb133cUL,
        0xeecea927UL, 0x35b761c9UL, 0xede11ce5UL, 0x3c7a47b1UL,
        0x599cd2dfUL, 0x3f55f273UL, 0x791814ceUL, 0xbf73c737UL,
        0xea53f7cdUL, 0x5b5ffdaaUL, 0x14df3d6fUL, 0x867844dbUL,
        0x81caaff3UL, 0x3eb968c4UL, 0x2c382434UL, 0x5fc2a340UL,
        0x72161dc3UL, 0x0cbce225UL, 0x8b283c49UL, 0x41ff0d95UL,
        0x7139a801UL, 0xde080cb3UL, 0x9cd8b4e4UL, 0x906456c1UL,
        0x617bcb84UL, 0x70d532b6UL, 0x74486c5cUL, 0x42d0b857UL,
    };

    static const u32 Td2[TableSize] =
    {
        0xa75051f4UL, 0x65537e41UL, 0xa4c31a17UL, 0x5e963a27UL,
        0x6bcb3babUL, 0x45f11f9dUL, 0x58abacfaUL, 0x03934be3UL,
        0xfa552030UL, 0x6df6ad76UL, 0x769188ccUL, 0x4c25f502UL,
        0xd7fc4fe5UL, 0xcbd7c52aUL, 0x44802635UL, 0xa38fb562UL,
        0x5a49deb1UL, 0x1b6725baUL, 0x0e9845eaUL, 0xc0e15dfeUL,
        0x7502c32fUL, 0xf012814cUL, 0x97a38d46UL, 0xf9c66bd3UL,
        0x5fe7038fUL, 0x9c951592UL, 0x7aebbf6dUL, 0x59da9552UL,
        0x832dd4beUL, 0x21d35874UL, 0x692949e0UL, 0xc8448ec9UL,
        0x896a75c2UL, 0x7978f48eUL, 0x3e6b9958UL, 0x71dd27b9UL,
        0x4fb6bee1UL, 0xad17f088UL, 0xac66c920UL, 0x3ab47dceUL,
        0x4a1863dfUL, 0x3182e51aUL, 0x33609751UL, 0x7f456253UL,
        0x77e0b164UL, 0xae84bb6bUL, 0xa01cfe81UL, 0x2b94f908UL,
        0x68587048UL, 0xfd198f45UL, 0x6c8794deUL, 0xf8b7527bUL,
        0xd323ab73UL, 0x02e2724bUL, 0x8f57e31fUL, 0xab2a6655UL,
        0x2807b2ebUL, 0xc2032fb5UL, 0x7b9a86c5UL, 0x08a5d337UL,
        0x87f23028UL, 0xa5b223bfUL, 0x6aba0203UL, 0x825ced16UL,
        0x1c2b8acfUL, 0xb492a779UL, 0xf2f0f307UL, 0xe2a14e69UL,
        0xf4cd65daUL, 0xbed50605UL, 0x621fd134UL, 0xfe8ac4a6UL,
        0x539d342eUL, 0x55a0a2f3UL, 0xe132058aUL, 0xeb75a4f6UL,
        0xec390b83UL, 0xefaa4060UL, 0x9f065e71UL, 0x1051bd6eUL,
        0x8af93e21UL, 0x063d96ddUL, 0x05aedd3eUL, 0xbd464de6UL,
        0x8db59154UL, 0x5d0571c4UL, 0xd46f0406UL, 0x15ff6050UL,
        0xfb241998UL, 0xe997d6bdUL, 0x43cc8940UL, 0x9e7767d9UL,
        0x42bdb0e8UL, 0x8b880789UL, 0x5b38e719UL, 0xeedb79c8UL,
        0x0a47a17cUL, 0x0fe97c42UL, 0x1ec9f884UL, 0x00000000UL,
        0x86830980UL, 0xed48322bUL, 0x70ac1e11UL, 0x724e6c5aUL,
        0xfffbfd0eUL, 0x38560f85UL, 0xd51e3daeUL, 0x3927362dUL,
        0xd9640a0fUL, 0xa621685cUL, 0x54d19b5bUL, 0x2e3a2436UL,
        0x67b10c0aUL, 0xe70f9357UL, 0x96d2b4eeUL, 0x919e1b9bUL,
        0xc54f80c0UL, 0x20a261dcUL, 0x4b695a77UL, 0x1a161c12UL,
        0xba0ae293UL, 0x2ae5c0a0UL, 0xe0433c22UL, 0x171d121bUL,
        0x0d0b0e09UL, 0xc7adf28bUL, 0xa8b92db6UL, 0xa9c8141eUL,
        0x198557f1UL, 0x074caf75UL, 0xddbbee99UL, 0x60fda37fUL,
        0x269ff701UL, 0xf5bc5c72UL, 0x3bc54466UL, 0x7e345bfbUL,
        0x29768b43UL, 0xc6dccb23UL, 0xfc68b6edUL, 0xf163b8e4UL,
        0xdccad731UL, 0x85104263UL, 0x22401397UL, 0x112084c6UL,
        0x247d854aUL, 0x3df8d2bbUL, 0x3211aef9UL, 0xa16dc729UL,
        0x2f4b1d9eUL, 0x30f3dcb2UL, 0x52ec0d86UL, 0xe3d077c1UL,
        0x166c2bb3UL, 0xb999a970UL, 0x48fa1194UL, 0x642247e9UL,
        0x8cc4a8fcUL, 0x3f1aa0f0UL, 0x2cd8567dUL, 0x90ef2233UL,
        0x4ec78749UL, 0xd1c1d938UL, 0xa2fe8ccaUL, 0x0b3698d4UL,
        0x81cfa6f5UL, 0xde28a57aUL, 0x8e26dab7UL, 0xbfa43fadUL,
        0x9de42c3aUL, 0x920d5078UL, 0xcc9b6a5fUL, 0x4662547eUL,
        0x13c2f68dUL, 0xb8e890d8UL, 0xf75e2e39UL, 0xaff582c3UL,
        0x80be9f5dUL, 0x937c69d0UL, 0x2da96fd5UL, 0x12b3cf25UL,
        0x993bc8acUL, 0x7da71018UL, 0x636ee89cUL, 0xbb7bdb3bUL,
        0x7809cd26UL, 0x18f46e59UL, 0xb701ec9aUL, 0x9aa8834fUL,
        0x6e65e695UL, 0xe67eaaffUL, 0xcf0821bcUL, 0xe8e6ef15UL,
        0x9bd9bae7UL, 0x36ce4a6fUL, 0x09d4ea9fUL, 0x7cd629b0UL,
        0xb2af31a4UL, 0x23312a3fUL, 0x9430c6a5UL, 0x66c035a2UL,
        0xbc37744eUL, 0xcaa6fc82UL, 0xd0b0e090UL, 0xd81533a7UL,
        0x984af104UL, 0xdaf741ecUL, 0x500e7fcdUL, 0xf62f1791UL,
        0xd68d764dUL, 0xb04d43efUL, 0x4d54ccaaUL, 0x04dfe496UL,
        0xb5e39ed1UL, 0x881b4c6aUL, 0x1fb8c12cUL, 0x517f4665UL,
        0xea049d5eUL, 0x355d018cUL, 0x7473fa87UL, 0x412efb0bUL,
        0x1d5ab367UL, 0xd25292dbUL, 0x5633e910UL, 0x47136dd6UL,
        0x618c9ad7UL, 0x0c7a37a1UL, 0x148e59f8UL, 0x3c89eb13UL,
        0x27eecea9UL, 0xc935b761UL, 0xe5ede11cUL, 0xb13c7a47UL,
        0xdf599cd2UL, 0x733f55f2UL, 0xce791814UL, 0x37bf73c7UL,
        0xcdea53f7UL, 0xaa5b5ffdUL, 0x6f14df3dUL, 0xdb867844UL,
        0xf381caafUL, 0xc43eb968UL, 0x342c3824UL, 0x405fc2a3UL,
        0xc372161dUL, 0x250cbce2UL, 0x498b283cUL, 0x9541ff0dUL,
        0x017139a8UL, 0xb3de080cUL, 0xe49cd8b4UL, 0xc1906456UL,
        0x84617bcbUL, 0xb670d532UL, 0x5c74486cUL, 0x5742d0b8UL,
    };

    static const u32 Td3[TableSize] =
    {
        0xf4a75051UL, 0x4165537eUL, 0x17a4c31aUL, 0x275e963aUL,
        0xab6bcb3bUL, 0x9d45f11fUL, 0xfa58abacUL, 0xe303934bUL,
        0x30fa5520UL, 0x766df6adUL, 0xcc769188UL, 0x024c25f5UL,
        0xe5d7fc4fUL, 0x2acbd7c5UL, 0x35448026UL, 0x62a38fb5UL,
        0xb15a49deUL, 0xba1b6725UL, 0xea0e9845UL, 0xfec0e15dUL,
        0x2f7502c3UL, 0x4cf01281UL, 0x4697a38dUL, 0xd3f9c66bUL,
        0x8f5fe703UL, 0x929c9515UL, 0x6d7aebbfUL, 0x5259da95UL,
        0xbe832dd4UL, 0x7421d358UL, 0xe0692949UL, 0xc9c8448eUL,
        0xc2896a75UL, 0x8e7978f4UL, 0x583e6b99UL, 0xb971dd27UL,
        0xe14fb6beUL, 0x88ad17f0UL, 0x20ac66c9UL, 0xce3ab47dUL,
        0xdf4a1863UL, 0x1a3182e5UL, 0x51336097UL, 0x537f4562UL,
        0x6477e0b1UL, 0x6bae84bbUL, 0x81a01cfeUL, 0x082b94f9UL,
        0x48685870UL, 0x45fd198fUL, 0xde6c8794UL, 0x7bf8b752UL,
        0x73d323abUL, 0x4b02e272UL, 0x1f8f57e3UL, 0x55ab2a66UL,
        0xeb2807b2UL, 0xb5c2032fUL, 0xc57b9a86UL, 0x3708a5d3UL,
        0x2887f230UL, 0xbfa5b223UL, 0x036aba02UL, 0x16825cedUL,
        0xcf1c2b8aUL, 0x79b492a7UL, 0x07f2f0f3UL, 0x69e2a14eUL,
        0xdaf4cd65UL, 0x05bed506UL, 0x34621fd1UL, 0xa6fe8ac4UL,
        0x2e539d34UL, 0xf355a0a2UL, 0x8ae13205UL, 0xf6eb75a4UL,
        0x83ec390bUL, 0x60efaa40UL, 0x719f065eUL, 0x6e1051bdUL,
        0x218af93eUL, 0xdd063d96UL, 0x3e05aeddUL, 0xe6bd464dUL,
        0x548db591UL, 0xc45d0571UL, 0x06d46f04UL, 0x5015ff60UL,
        0x98fb2419UL, 0xbde997d6UL, 0x4043cc89UL, 0xd99e7767UL,
        0xe842bdb0UL, 0x898b8807UL, 0x195b38e7UL, 0xc8eedb79UL,
        0x7c0a47a1UL, 0x420fe97cUL, 0x841ec9f8UL, 0x00000000UL,
        0x80868309UL, 0x2bed4832UL, 0x1170ac1eUL, 0x5a724e6cUL,
        0x0efffbfdUL, 0x8538560fUL, 0xaed51e3dUL, 0x2d392736UL,
        0x0fd9640aUL, 0x5ca62168UL, 0x5b54d19bUL, 0x362e3a24UL,
        0x0a67b10cUL, 0x57e70f93UL, 0xee96d2b4UL, 0x9b919e1bUL,
        0xc0c54f80UL, 0xdc20a261UL, 0x774b695aUL, 0x121a161cUL,
        0x93ba0ae2UL, 0xa02ae5c0UL, 0x22e0433cUL, 0x1b171d12UL,
        0x090d0b0eUL, 0x8bc7adf2UL, 0xb6a8b92dUL, 0x1ea9c814UL,
        0xf1198557UL, 0x75074cafUL, 0x99ddbbeeUL, 0x7f60fda3UL,
        0x01269ff7UL, 0x72f5bc5cUL, 0x663bc544UL, 0xfb7e345bUL,
        0x4329768bUL, 0x23c6dccbUL, 0xedfc68b6UL, 0xe4f163b8UL,
        0x31dccad7UL, 0x63851042UL, 0x97224013UL, 0xc6112084UL,
        0x4a247d85UL, 0xbb3df8d2UL, 0xf93211aeUL, 0x29a16dc7UL,
        0x9e2f4b1dUL, 0xb230f3dcUL, 0x8652ec0dUL, 0xc1e3d077UL,
        0xb3166c2bUL, 0x70b999a9UL, 0x9448fa11UL, 0xe9642247UL,
        0xfc8cc4a8UL, 0xf03f1aa0UL, 0x7d2cd856UL, 0x3390ef22UL,
        0x494ec787UL, 0x38d1c1d9UL, 0xcaa2fe8cUL, 0xd40b3698UL,
        0xf581cfa6UL, 0x7ade28a5UL, 0xb78e26daUL, 0xadbfa43fUL,
        0x3a9de42cUL, 0x78920d50UL, 0x5fcc9b6aUL, 0x7e466254UL,
        0x8d13c2f6UL, 0xd8b8e890UL, 0x39f75e2eUL, 0xc3aff582UL,
        0x5d80be9fUL, 0xd0937c69UL, 0xd52da96fUL, 0x2512b3cfUL,
        0xac993bc8UL, 0x187da710UL, 0x9c636ee8UL, 0x3bbb7bdbUL,
        0x267809cdUL, 0x5918f46eUL, 0x9ab701ecUL, 0x4f9aa883UL,
        0x956e65e6UL, 0xffe67eaaUL, 0xbccf0821UL, 0x15e8e6efUL,
        0xe79bd9baUL, 0x6f36ce4aUL, 0x9f09d4eaUL, 0xb07cd629UL,
        0xa4b2af31UL, 0x3f23312aUL, 0xa59430c6UL, 0xa266c035UL,
        0x4ebc3774UL, 0x82caa6fcUL, 0x90d0b0e0UL, 0xa7d81533UL,
        0x04984af1UL, 0xecdaf741UL, 0xcd500e7fUL, 0x91f62f17UL,
        0x4dd68d76UL, 0xefb04d43UL, 0xaa4d54ccUL, 0x9604dfe4UL,
        0xd1b5e39eUL, 0x6a881b4cUL, 0x2c1fb8c1UL, 0x65517f46UL,
        0x5eea049dUL, 0x8c355d01UL, 0x877473faUL, 0x0b412efbUL,
        0x671d5ab3UL, 0xdbd25292UL, 0x105633e9UL, 0xd647136dUL,
        0xd7618c9aUL, 0xa10c7a37UL, 0xf8148e59UL, 0x133c89ebUL,
        0xa927eeceUL, 0x61c935b7UL, 0x1ce5ede1UL, 0x47b13c7aUL,
        0xd2df599cUL, 0xf2733f55UL, 0x14ce7918UL, 0xc737bf73UL,
        0xf7cdea53UL, 0xfdaa5b5fUL, 0x3d6f14dfUL, 0x44db8678UL,
        0xaff381caUL, 0x68c43eb9UL, 0x24342c38UL, 0xa3405fc2UL,
        0x1dc37216UL, 0xe2250cbcUL, 0x3c498b28UL, 0x0d9541ffUL,
        0xa8017139UL, 0x0cb3de08UL, 0xb4e49cd8UL, 0x56c19064UL,
        0xcb84617bUL, 0x32b670d5UL, 0x6c5c7448UL, 0xb85742d0UL,
    };

    static const u32 Td4[TableSize] =
    {
        0x52525252UL, 0x09090909UL, 0x6a6a6a6aUL, 0xd5d5d5d5UL,
        0x30303030UL, 0x36363636UL, 0xa5a5a5a5UL, 0x38383838UL,
        0xbfbfbfbfUL, 0x40404040UL, 0xa3a3a3a3UL, 0x9e9e9e9eUL,
        0x81818181UL, 0xf3f3f3f3UL, 0xd7d7d7d7UL, 0xfbfbfbfbUL,
        0x7c7c7c7cUL, 0xe3e3e3e3UL, 0x39393939UL, 0x82828282UL,
        0x9b9b9b9bUL, 0x2f2f2f2fUL, 0xffffffffUL, 0x87878787UL,
        0x34343434UL, 0x8e8e8e8eUL, 0x43434343UL, 0x44444444UL,
        0xc4c4c4c4UL, 0xdedededeUL, 0xe9e9e9e9UL, 0xcbcbcbcbUL,
        0x54545454UL, 0x7b7b7b7bUL, 0x94949494UL, 0x32323232UL,
        0xa6a6a6a6UL, 0xc2c2c2c2UL, 0x23232323UL, 0x3d3d3d3dUL,
        0xeeeeeeeeUL, 0x4c4c4c4cUL, 0x95959595UL, 0x0b0b0b0bUL,
        0x42424242UL, 0xfafafafaUL, 0xc3c3c3c3UL, 0x4e4e4e4eUL,
        0x08080808UL, 0x2e2e2e2eUL, 0xa1a1a1a1UL, 0x66666666UL,
        0x28282828UL, 0xd9d9d9d9UL, 0x24242424UL, 0xb2b2b2b2UL,
        0x76767676UL, 0x5b5b5b5bUL, 0xa2a2a2a2UL, 0x49494949UL,
        0x6d6d6d6dUL, 0x8b8b8b8bUL, 0xd1d1d1d1UL, 0x25252525UL,
        0x72727272UL, 0xf8f8f8f8UL, 0xf6f6f6f6UL, 0x64646464UL,
        0x86868686UL, 0x68686868UL, 0x98989898UL, 0x16161616UL,
        0xd4d4d4d4UL, 0xa4a4a4a4UL, 0x5c5c5c5cUL, 0xccccccccUL,
        0x5d5d5d5dUL, 0x65656565UL, 0xb6b6b6b6UL, 0x92929292UL,
        0x6c6c6c6cUL, 0x70707070UL, 0x48484848UL, 0x50505050UL,
        0xfdfdfdfdUL, 0xededededUL, 0xb9b9b9b9UL, 0xdadadadaUL,
        0x5e5e5e5eUL, 0x15151515UL, 0x46464646UL, 0x57575757UL,
        0xa7a7a7a7UL, 0x8d8d8d8dUL, 0x9d9d9d9dUL, 0x84848484UL,
        0x90909090UL, 0xd8d8d8d8UL, 0xababababUL, 0x00000000UL,
        0x8c8c8c8cUL, 0xbcbcbcbcUL, 0xd3d3d3d3UL, 0x0a0a0a0aUL,
        0xf7f7f7f7UL, 0xe4e4e4e4UL, 0x58585858UL, 0x05050505UL,
        0xb8b8b8b8UL, 0xb3b3b3b3UL, 0x45454545UL, 0x06060606UL,
        0xd0d0d0d0UL, 0x2c2c2c2cUL, 0x1e1e1e1eUL, 0x8f8f8f8fUL,
        0xcacacacaUL, 0x3f3f3f3fUL, 0x0f0f0f0fUL, 0x02020202UL,
        0xc1c1c1c1UL, 0xafafafafUL, 0xbdbdbdbdUL, 0x03030303UL,
        0x01010101UL, 0x13131313UL, 0x8a8a8a8aUL, 0x6b6b6b6bUL,
        0x3a3a3a3aUL, 0x91919191UL, 0x11111111UL, 0x41414141UL,
        0x4f4f4f4fUL, 0x67676767UL, 0xdcdcdcdcUL, 0xeaeaeaeaUL,
        0x97979797UL, 0xf2f2f2f2UL, 0xcfcfcfcfUL, 0xcecececeUL,
        0xf0f0f0f0UL, 0xb4b4b4b4UL, 0xe6e6e6e6UL, 0x73737373UL,
        0x96969696UL, 0xacacacacUL, 0x74747474UL, 0x22222222UL,
        0xe7e7e7e7UL, 0xadadadadUL, 0x35353535UL, 0x85858585UL,
        0xe2e2e2e2UL, 0xf9f9f9f9UL, 0x37373737UL, 0xe8e8e8e8UL,
        0x1c1c1c1cUL, 0x75757575UL, 0xdfdfdfdfUL, 0x6e6e6e6eUL,
        0x47474747UL, 0xf1f1f1f1UL, 0x1a1a1a1aUL, 0x71717171UL,
        0x1d1d1d1dUL, 0x29292929UL, 0xc5c5c5c5UL, 0x89898989UL,
        0x6f6f6f6fUL, 0xb7b7b7b7UL, 0x62626262UL, 0x0e0e0e0eUL,
        0xaaaaaaaaUL, 0x18181818UL, 0xbebebebeUL, 0x1b1b1b1bUL,
        0xfcfcfcfcUL, 0x56565656UL, 0x3e3e3e3eUL, 0x4b4b4b4bUL,
        0xc6c6c6c6UL, 0xd2d2d2d2UL, 0x79797979UL, 0x20202020UL,
        0x9a9a9a9aUL, 0xdbdbdbdbUL, 0xc0c0c0c0UL, 0xfefefefeUL,
        0x78787878UL, 0xcdcdcdcdUL, 0x5a5a5a5aUL, 0xf4f4f4f4UL,
        0x1f1f1f1fUL, 0xddddddddUL, 0xa8a8a8a8UL, 0x33333333UL,
        0x88888888UL, 0x07070707UL, 0xc7c7c7c7UL, 0x31313131UL,
        0xb1b1b1b1UL, 0x12121212UL, 0x10101010UL, 0x59595959UL,
        0x27272727UL, 0x80808080UL, 0xececececUL, 0x5f5f5f5fUL,
        0x60606060UL, 0x51515151UL, 0x7f7f7f7fUL, 0xa9a9a9a9UL,
        0x19191919UL, 0xb5b5b5b5UL, 0x4a4a4a4aUL, 0x0d0d0d0dUL,
        0x2d2d2d2dUL, 0xe5e5e5e5UL, 0x7a7a7a7aUL, 0x9f9f9f9fUL,
        0x93939393UL, 0xc9c9c9c9UL, 0x9c9c9c9cUL, 0xefefefefUL,
        0xa0a0a0a0UL, 0xe0e0e0e0UL, 0x3b3b3b3bUL, 0x4d4d4d4dUL,
        0xaeaeaeaeUL, 0x2a2a2a2aUL, 0xf5f5f5f5UL, 0xb0b0b0b0UL,
        0xc8c8c8c8UL, 0xebebebebUL, 0xbbbbbbbbUL, 0x3c3c3c3cUL,
        0x83838383UL, 0x53535353UL, 0x99999999UL, 0x61616161UL,
        0x17171717UL, 0x2b2b2b2bUL, 0x04040404UL, 0x7e7e7e7eUL,
        0xbabababaUL, 0x77777777UL, 0xd6d6d6d6UL, 0x26262626UL,
        0xe1e1e1e1UL, 0x69696969UL, 0x14141414UL, 0x63636363UL,
        0x55555555UL, 0x21212121UL, 0x0c0c0c0cUL, 0x7d7d7d7dUL,
    };

    static const u32 rcon[] =
    {
        0x01000000UL, 0x02000000UL, 0x04000000UL, 0x08000000UL,
        0x10000000UL, 0x20000000UL, 0x40000000UL, 0x80000000UL,
        0x1B000000UL, 0x36000000UL,
    };

#define GETU32(pt) (((unsigned int)(pt)[0] << 24) ^ \
    ((unsigned int)(pt)[1] << 16) ^ \
    ((unsigned int)(pt)[2] <<  8) ^ \
    ((unsigned int)(pt)[3]))

#define PUTU32(ct, st) { (ct)[0] = (unsigned char)((st) >> 24); \
    (ct)[1] = (unsigned char)((st) >> 16); \
    (ct)[2] = (unsigned char)((st) >>  8); \
    (ct)[3] = (unsigned char)(st); }


    u8 setKeyEnc(u32* rk, const u8* cipherKey, s32 keyBytes)
    {
        s32 i = 0;
        u32 tmp;

        rk[0] = GETU32(cipherKey     );
        rk[1] = GETU32(cipherKey +  4);
        rk[2] = GETU32(cipherKey +  8);
        rk[3] = GETU32(cipherKey + 12);
        if(keyBytes == 16){ // 128 bits
            for(;;){
                tmp  = rk[3];
                rk[4] = rk[0] ^
                    (Te4[(tmp >> 16) & 0xff] & 0xff000000) ^
                    (Te4[(tmp >>  8) & 0xff] & 0x00ff0000) ^
                    (Te4[(tmp      ) & 0xff] & 0x0000ff00) ^
                    (Te4[(tmp >> 24)       ] & 0x000000ff) ^
                    rcon[i];
                rk[5] = rk[1] ^ rk[4];
                rk[6] = rk[2] ^ rk[5];
                rk[7] = rk[3] ^ rk[6];
                if (++i == 10) {
                    return 10;
                }
                rk += 4;
            }
        }
        rk[4] = GETU32(cipherKey + 16);
        rk[5] = GETU32(cipherKey + 20);
        if(keyBytes == 24){ // 192 bits
            for(;;){
                tmp = rk[ 5];
                rk[ 6] = rk[ 0] ^
                    (Te4[(tmp >> 16) & 0xff] & 0xff000000) ^
                    (Te4[(tmp >>  8) & 0xff] & 0x00ff0000) ^
                    (Te4[(tmp      ) & 0xff] & 0x0000ff00) ^
                    (Te4[(tmp >> 24)       ] & 0x000000ff) ^
                    rcon[i];
                rk[ 7] = rk[ 1] ^ rk[ 6];
                rk[ 8] = rk[ 2] ^ rk[ 7];
                rk[ 9] = rk[ 3] ^ rk[ 8];
                if (++i == 8) {
                    return 12;
                }
                rk[10] = rk[ 4] ^ rk[ 9];
                rk[11] = rk[ 5] ^ rk[10];
                rk += 6;
            }
        }
        rk[6] = GETU32(cipherKey + 24);
        rk[7] = GETU32(cipherKey + 28);
        if(keyBytes == 32){ // 256 bits
            for(;;){
                tmp = rk[ 7];
                rk[ 8] = rk[ 0] ^
                    (Te4[(tmp >> 16) & 0xff] & 0xff000000) ^
                    (Te4[(tmp >>  8) & 0xff] & 0x00ff0000) ^
                    (Te4[(tmp      ) & 0xff] & 0x0000ff00) ^
                    (Te4[(tmp >> 24)       ] & 0x000000ff) ^
                    rcon[i];
                rk[ 9] = rk[ 1] ^ rk[ 8];
                rk[10] = rk[ 2] ^ rk[ 9];
                rk[11] = rk[ 3] ^ rk[10];
                if(++i == 7){
                    return 14;
                }
                tmp = rk[11];
                rk[12] = rk[ 4] ^
                    (Te4[(tmp >> 24)       ] & 0xff000000) ^
                    (Te4[(tmp >> 16) & 0xff] & 0x00ff0000) ^
                    (Te4[(tmp >>  8) & 0xff] & 0x0000ff00) ^
                    (Te4[(tmp      ) & 0xff] & 0x000000ff);
                rk[13] = rk[ 5] ^ rk[12];
                rk[14] = rk[ 6] ^ rk[13];
                rk[15] = rk[ 7] ^ rk[14];

                rk += 8;
            }
        }
        return 0;
    }


    u8 generateKeySchedule(u32* rk, u32* rrk, const u8* cipherKey, s32 keyBytes)
    {
        u8 Nr = setKeyEnc(rk, cipherKey, keyBytes);

        rrk += Nr*4;
        rrk[0] = rk[0];
        rrk[1] = rk[1];
        rrk[2] = rk[2];
        rrk[3] = rk[3];

        for(s32 i=1; i<Nr; ++i){
            rrk -= 4;
            rk += 4;
            rrk[0] =
                Td0[Te4[(rk[0] >> 24)       ] & 0xff] ^
                Td1[Te4[(rk[0] >> 16) & 0xff] & 0xff] ^
                Td2[Te4[(rk[0] >>  8) & 0xff] & 0xff] ^
                Td3[Te4[(rk[0]      ) & 0xff] & 0xff];
            rrk[1] =
                Td0[Te4[(rk[1] >> 24)       ] & 0xff] ^
                Td1[Te4[(rk[1] >> 16) & 0xff] & 0xff] ^
                Td2[Te4[(rk[1] >>  8) & 0xff] & 0xff] ^
                Td3[Te4[(rk[1]      ) & 0xff] & 0xff];
            rrk[2] =
                Td0[Te4[(rk[2] >> 24)       ] & 0xff] ^
                Td1[Te4[(rk[2] >> 16) & 0xff] & 0xff] ^
                Td2[Te4[(rk[2] >>  8) & 0xff] & 0xff] ^
                Td3[Te4[(rk[2]      ) & 0xff] & 0xff];
            rrk[3] =
                Td0[Te4[(rk[3] >> 24)       ] & 0xff] ^
                Td1[Te4[(rk[3] >> 16) & 0xff] & 0xff] ^
                Td2[Te4[(rk[3] >>  8) & 0xff] & 0xff] ^
                Td3[Te4[(rk[3]      ) & 0xff] & 0xff];
        }

        rrk -= 4;
        rk += 4;
        rrk[0] = rk[0];
        rrk[1] = rk[1];
        rrk[2] = rk[2];
        rrk[3] = rk[3];

        return Nr;
    }


    void encryptBlock(const AESContext& context, u32* iv, u8* ct, const u8* pt)
    {
        u32 s0, s1, s2, s3, t0, t1, t2, t3;
        s32 r;

        const u32* rk = context.Ek_;

        s0 = GETU32(pt     ) ^ rk[0];
        s1 = GETU32(pt +  4) ^ rk[1];
        s2 = GETU32(pt +  8) ^ rk[2];
        s3 = GETU32(pt + 12) ^ rk[3];
        if(AES::Mode_ECB != context.mode_){
            s0 = s0 ^ iv[0];
            s1 = s1 ^ iv[1];
            s2 = s2 ^ iv[2];
            s3 = s3 ^ iv[3];
        }


        r = context.Nr_ >> 1;
        for(;;){
            t0 =
                Te0[(s0 >> 24)       ] ^
                Te1[(s1 >> 16) & 0xff] ^
                Te2[(s2 >>  8) & 0xff] ^
                Te3[(s3      ) & 0xff] ^
                rk[4];
            t1 =
                Te0[(s1 >> 24)       ] ^
                Te1[(s2 >> 16) & 0xff] ^
                Te2[(s3 >>  8) & 0xff] ^
                Te3[(s0      ) & 0xff] ^
                rk[5];
            t2 =
                Te0[(s2 >> 24)       ] ^
                Te1[(s3 >> 16) & 0xff] ^
                Te2[(s0 >>  8) & 0xff] ^
                Te3[(s1      ) & 0xff] ^
                rk[6];
            t3 =
                Te0[(s3 >> 24)       ] ^
                Te1[(s0 >> 16) & 0xff] ^
                Te2[(s1 >>  8) & 0xff] ^
                Te3[(s2      ) & 0xff] ^
                rk[7];

            rk += 8;
            if (--r == 0) {
                break;
            }

            s0 =
                Te0[(t0 >> 24)       ] ^
                Te1[(t1 >> 16) & 0xff] ^
                Te2[(t2 >>  8) & 0xff] ^
                Te3[(t3      ) & 0xff] ^
                rk[0];
            s1 =
                Te0[(t1 >> 24)       ] ^
                Te1[(t2 >> 16) & 0xff] ^
                Te2[(t3 >>  8) & 0xff] ^
                Te3[(t0      ) & 0xff] ^
                rk[1];
            s2 =
                Te0[(t2 >> 24)       ] ^
                Te1[(t3 >> 16) & 0xff] ^
                Te2[(t0 >>  8) & 0xff] ^
                Te3[(t1      ) & 0xff] ^
                rk[2];
            s3 =
                Te0[(t3 >> 24)       ] ^
                Te1[(t0 >> 16) & 0xff] ^
                Te2[(t1 >>  8) & 0xff] ^
                Te3[(t2      ) & 0xff] ^
                rk[3];
        }


        s0 =
            (Te4[(t0 >> 24)       ] & 0xff000000) ^
            (Te4[(t1 >> 16) & 0xff] & 0x00ff0000) ^
            (Te4[(t2 >>  8) & 0xff] & 0x0000ff00) ^
            (Te4[(t3      ) & 0xff] & 0x000000ff) ^
            rk[0];
        PUTU32(ct     , s0);
        s1 =
            (Te4[(t1 >> 24)       ] & 0xff000000) ^
            (Te4[(t2 >> 16) & 0xff] & 0x00ff0000) ^
            (Te4[(t3 >>  8) & 0xff] & 0x0000ff00) ^
            (Te4[(t0      ) & 0xff] & 0x000000ff) ^
            rk[1];
        PUTU32(ct +  4, s1);
        s2 =
            (Te4[(t2 >> 24)       ] & 0xff000000) ^
            (Te4[(t3 >> 16) & 0xff] & 0x00ff0000) ^
            (Te4[(t0 >>  8) & 0xff] & 0x0000ff00) ^
            (Te4[(t1      ) & 0xff] & 0x000000ff) ^
            rk[2];
        PUTU32(ct +  8, s2);
        s3 =
            (Te4[(t3 >> 24)       ] & 0xff000000) ^
            (Te4[(t0 >> 16) & 0xff] & 0x00ff0000) ^
            (Te4[(t1 >>  8) & 0xff] & 0x0000ff00) ^
            (Te4[(t2      ) & 0xff] & 0x000000ff) ^
            rk[3];
        PUTU32(ct + 12, s3);

        if(AES::Mode_ECB != context.mode_){
            iv[0] = s0;
            iv[1] = s1;
            iv[2] = s2;
            iv[3] = s3;
        }
    }


    void decryptBlock(const AESContext& context, u32* iv, u8* pt, const u8* ct)
    {
        u32 s0, s1, s2, s3, t0, t1, t2, t3, v0, v1, v2, v3;
        s32 r;
        const u32* rk = context.Dk_;

        v0 = GETU32(ct     );    s0 = v0 ^ rk[0];
        v1 = GETU32(ct +  4);    s1 = v1 ^ rk[1];
        v2 = GETU32(ct +  8);    s2 = v2 ^ rk[2];
        v3 = GETU32(ct + 12);    s3 = v3 ^ rk[3];

        r = context.Nr_ >> 1;
        for(;;){
            t0 =
                Td0[(s0 >> 24)       ] ^
                Td1[(s3 >> 16) & 0xff] ^
                Td2[(s2 >>  8) & 0xff] ^
                Td3[(s1      ) & 0xff] ^
                rk[4];
            t1 =
                Td0[(s1 >> 24)       ] ^
                Td1[(s0 >> 16) & 0xff] ^
                Td2[(s3 >>  8) & 0xff] ^
                Td3[(s2      ) & 0xff] ^
                rk[5];
            t2 =
                Td0[(s2 >> 24)       ] ^
                Td1[(s1 >> 16) & 0xff] ^
                Td2[(s0 >>  8) & 0xff] ^
                Td3[(s3      ) & 0xff] ^
                rk[6];
            t3 =
                Td0[(s3 >> 24)       ] ^
                Td1[(s2 >> 16) & 0xff] ^
                Td2[(s1 >>  8) & 0xff] ^
                Td3[(s0      ) & 0xff] ^
                rk[7];

            rk += 8;
            if (--r == 0) {
                break;
            }

            s0 =
                Td0[(t0 >> 24)       ] ^
                Td1[(t3 >> 16) & 0xff] ^
                Td2[(t2 >>  8) & 0xff] ^
                Td3[(t1      ) & 0xff] ^
                rk[0];
            s1 =
                Td0[(t1 >> 24)       ] ^
                Td1[(t0 >> 16) & 0xff] ^
                Td2[(t3 >>  8) & 0xff] ^
                Td3[(t2      ) & 0xff] ^
                rk[1];
            s2 =
                Td0[(t2 >> 24)       ] ^
                Td1[(t1 >> 16) & 0xff] ^
                Td2[(t0 >>  8) & 0xff] ^
                Td3[(t3      ) & 0xff] ^
                rk[2];
            s3 =
                Td0[(t3 >> 24)       ] ^
                Td1[(t2 >> 16) & 0xff] ^
                Td2[(t1 >>  8) & 0xff] ^
                Td3[(t0      ) & 0xff] ^
                rk[3];
        }


        s0 =
            (Td4[(t0 >> 24)       ] & 0xff000000) ^
            (Td4[(t3 >> 16) & 0xff] & 0x00ff0000) ^
            (Td4[(t2 >>  8) & 0xff] & 0x0000ff00) ^
            (Td4[(t1      ) & 0xff] & 0x000000ff) ^
            rk[0];
        s1 =
            (Td4[(t1 >> 24)       ] & 0xff000000) ^
            (Td4[(t0 >> 16) & 0xff] & 0x00ff0000) ^
            (Td4[(t3 >>  8) & 0xff] & 0x0000ff00) ^
            (Td4[(t2      ) & 0xff] & 0x000000ff) ^
            rk[1];
        s2 =
            (Td4[(t2 >> 24)       ] & 0xff000000) ^
            (Td4[(t1 >> 16) & 0xff] & 0x00ff0000) ^
            (Td4[(t0 >>  8) & 0xff] & 0x0000ff00) ^
            (Td4[(t3      ) & 0xff] & 0x000000ff) ^
            rk[2];
        s3 =
            (Td4[(t3 >> 24)       ] & 0xff000000) ^
            (Td4[(t2 >> 16) & 0xff] & 0x00ff0000) ^
            (Td4[(t1 >>  8) & 0xff] & 0x0000ff00) ^
            (Td4[(t0      ) & 0xff] & 0x000000ff) ^
            rk[3];

        if(AES::Mode_ECB != context.mode_){
            s0 = s0 ^ iv[0]; iv[0] = v0;
            s1 = s1 ^ iv[1]; iv[1] = v1;
            s2 = s2 ^ iv[2]; iv[2] = v2;
            s3 = s3 ^ iv[3]; iv[3] = v3;
        }

        PUTU32(pt     , s0);
        PUTU32(pt +  4, s1);
        PUTU32(pt +  8, s2);
        PUTU32(pt + 12, s3);
    }

 }

    AESContext::AESContext()
        :mode_(AES::Mode_None)
    {
        for(u32 i=0; i<4; ++i){
            IV_[i] = 0;
        }
    }

    AESContext::AESContext(const u8* IV, const u8* key, u32 keyLength, AES::Mode mode)
        :mode_(AES::Mode_None)
    {
        for(u32 i=0; i<4; ++i){
            IV_[i] = 0;
        }

        initialize(IV, key, keyLength, mode);
    }

    AESContext::~AESContext()
    {
    }

    bool AESContext::isInitialized() const
    {
        return (AES::Mode_None != mode_);
    }

    bool AESContext::initialize(const u8* IV, const u8* key, u32 keyLength, AES::Mode mode)
    {
        if(NULL == key){
            return false;
        }
        if(AES::Key128Bytes != keyLength
            && AES::Key192Bytes != keyLength
            && AES::Key256Bytes != keyLength)
        {
            return false;
        }
        Nr_ = generateKeySchedule(Ek_, Dk_, key, keyLength);

        if(NULL != IV){
            IV_[0] = GETU32(IV);
            IV_[1] = GETU32(IV + 4);
            IV_[2] = GETU32(IV + 8);
            IV_[3] = GETU32(IV + 12);
        }
        mode_ = static_cast<s8>(mode);

        return true;
    }

    s32 AES::encrypt(const AESContext& context, u8* cipher, const u8* data, s32 dataLength)
    {
        LASSERT(NULL != cipher);
        LASSERT(NULL != data);
        LASSERT(PKCS5::checkLength(dataLength, BlockSizeInBytes));
        LASSERT(context.isInitialized());

        u32 IV[4];
        lcore::memcpy(IV, context.IV_, sizeof(u32)*4);

        for(s32 i=0; i<dataLength; i+=AES::BlockSizeInBytes){
            encryptBlock(context, IV, cipher, data);
            cipher += AES::BlockSizeInBytes;
            data += AES::BlockSizeInBytes;
        }
        return dataLength;
    }

    s32 AES::decrypt(const AESContext& context, u8* data, const u8* cipher, s32 cipherLength)
    {
        LASSERT(NULL != data);
        LASSERT(NULL != cipher);
        LASSERT(PKCS5::checkLength(cipherLength, BlockSizeInBytes));
        LASSERT(context.isInitialized());

        u32 IV[4];
        lcore::memcpy(IV, context.IV_, sizeof(u32)*4);

        for(s32 i=0; i<cipherLength; i+=AES::BlockSizeInBytes){
            decryptBlock(context, IV, data, cipher);
            data += AES::BlockSizeInBytes;
            cipher += AES::BlockSizeInBytes;
        }
        return cipherLength;
    }

    //----------------------------------------------------
    //---
    //--- BlowFish
    //---
    //----------------------------------------------------
// from blowfish.cpp   C++ class implementation of the BLOWFISH encryption algorithm
// _THE BLOWFISH ENCRYPTION ALGORITHM_
// by Bruce Schneier
// Revised code--3/20/94
// Converted to C++ class 5/96, Jim Conger
namespace
{
    static u32 bf_P[BlowFish::NPass + 2] =
    {
        0x243f6a88, 0x85a308d3, 0x13198a2e, 0x03707344,
        0xa4093822, 0x299f31d0, 0x082efa98, 0xec4e6c89,
        0x452821e6, 0x38d01377, 0xbe5466cf, 0x34e90c6c,
        0xc0ac29b7, 0xc97c50dd, 0x3f84d5b5, 0xb5470917,
        0x9216d5d9, 0x8979fb1b,
    };

    static u32 bf_S[4][256] =
    {
        0xd1310ba6, 0x98dfb5ac, 0x2ffd72db, 0xd01adfb7,
        0xb8e1afed, 0x6a267e96, 0xba7c9045, 0xf12c7f99,
        0x24a19947, 0xb3916cf7, 0x0801f2e2, 0x858efc16,
        0x636920d8, 0x71574e69, 0xa458fea3, 0xf4933d7e,
        0x0d95748f, 0x728eb658, 0x718bcd58, 0x82154aee,
        0x7b54a41d, 0xc25a59b5, 0x9c30d539, 0x2af26013,
        0xc5d1b023, 0x286085f0, 0xca417918, 0xb8db38ef,
        0x8e79dcb0, 0x603a180e, 0x6c9e0e8b, 0xb01e8a3e,
        0xd71577c1, 0xbd314b27, 0x78af2fda, 0x55605c60,
        0xe65525f3, 0xaa55ab94, 0x57489862, 0x63e81440,
        0x55ca396a, 0x2aab10b6, 0xb4cc5c34, 0x1141e8ce,
        0xa15486af, 0x7c72e993, 0xb3ee1411, 0x636fbc2a,
        0x2ba9c55d, 0x741831f6, 0xce5c3e16, 0x9b87931e,
        0xafd6ba33, 0x6c24cf5c, 0x7a325381, 0x28958677,
        0x3b8f4898, 0x6b4bb9af, 0xc4bfe81b, 0x66282193,
        0x61d809cc, 0xfb21a991, 0x487cac60, 0x5dec8032,
        0xef845d5d, 0xe98575b1, 0xdc262302, 0xeb651b88,
        0x23893e81, 0xd396acc5, 0x0f6d6ff3, 0x83f44239,
        0x2e0b4482, 0xa4842004, 0x69c8f04a, 0x9e1f9b5e,
        0x21c66842, 0xf6e96c9a, 0x670c9c61, 0xabd388f0,
        0x6a51a0d2, 0xd8542f68, 0x960fa728, 0xab5133a3,
        0x6eef0b6c, 0x137a3be4, 0xba3bf050, 0x7efb2a98,
        0xa1f1651d, 0x39af0176, 0x66ca593e, 0x82430e88,
        0x8cee8619, 0x456f9fb4, 0x7d84a5c3, 0x3b8b5ebe,
        0xe06f75d8, 0x85c12073, 0x401a449f, 0x56c16aa6,
        0x4ed3aa62, 0x363f7706, 0x1bfedf72, 0x429b023d,
        0x37d0d724, 0xd00a1248, 0xdb0fead3, 0x49f1c09b,
        0x075372c9, 0x80991b7b, 0x25d479d8, 0xf6e8def7,
        0xe3fe501a, 0xb6794c3b, 0x976ce0bd, 0x04c006ba,
        0xc1a94fb6, 0x409f60c4, 0x5e5c9ec2, 0x196a2463,
        0x68fb6faf, 0x3e6c53b5, 0x1339b2eb, 0x3b52ec6f,
        0x6dfc511f, 0x9b30952c, 0xcc814544, 0xaf5ebd09,
        0xbee3d004, 0xde334afd, 0x660f2807, 0x192e4bb3,
        0xc0cba857, 0x45c8740f, 0xd20b5f39, 0xb9d3fbdb,
        0x5579c0bd, 0x1a60320a, 0xd6a100c6, 0x402c7279,
        0x679f25fe, 0xfb1fa3cc, 0x8ea5e9f8, 0xdb3222f8,
        0x3c7516df, 0xfd616b15, 0x2f501ec8, 0xad0552ab,
        0x323db5fa, 0xfd238760, 0x53317b48, 0x3e00df82,
        0x9e5c57bb, 0xca6f8ca0, 0x1a87562e, 0xdf1769db,
        0xd542a8f6, 0x287effc3, 0xac6732c6, 0x8c4f5573,
        0x695b27b0, 0xbbca58c8, 0xe1ffa35d, 0xb8f011a0,
        0x10fa3d98, 0xfd2183b8, 0x4afcb56c, 0x2dd1d35b,
        0x9a53e479, 0xb6f84565, 0xd28e49bc, 0x4bfb9790,
        0xe1ddf2da, 0xa4cb7e33, 0x62fb1341, 0xcee4c6e8,
        0xef20cada, 0x36774c01, 0xd07e9efe, 0x2bf11fb4,
        0x95dbda4d, 0xae909198, 0xeaad8e71, 0x6b93d5a0,
        0xd08ed1d0, 0xafc725e0, 0x8e3c5b2f, 0x8e7594b7,
        0x8ff6e2fb, 0xf2122b64, 0x8888b812, 0x900df01c,
        0x4fad5ea0, 0x688fc31c, 0xd1cff191, 0xb3a8c1ad,
        0x2f2f2218, 0xbe0e1777, 0xea752dfe, 0x8b021fa1,
        0xe5a0cc0f, 0xb56f74e8, 0x18acf3d6, 0xce89e299,
        0xb4a84fe0, 0xfd13e0b7, 0x7cc43b81, 0xd2ada8d9,
        0x165fa266, 0x80957705, 0x93cc7314, 0x211a1477,
        0xe6ad2065, 0x77b5fa86, 0xc75442f5, 0xfb9d35cf,
        0xebcdaf0c, 0x7b3e89a0, 0xd6411bd3, 0xae1e7e49,
        0x00250e2d, 0x2071b35e, 0x226800bb, 0x57b8e0af,
        0x2464369b, 0xf009b91e, 0x5563911d, 0x59dfa6aa,
        0x78c14389, 0xd95a537f, 0x207d5ba2, 0x02e5b9c5,
        0x83260376, 0x6295cfa9, 0x11c81968, 0x4e734a41,
        0xb3472dca, 0x7b14a94a, 0x1b510052, 0x9a532915,
        0xd60f573f, 0xbc9bc6e4, 0x2b60a476, 0x81e67400,
        0x08ba6fb5, 0x571be91f, 0xf296ec6b, 0x2a0dd915,
        0xb6636521, 0xe7b9f9b6, 0xff34052e, 0xc5855664,
        0x53b02d5d, 0xa99f8fa1, 0x08ba4799, 0x6e85076a,
        0x4b7a70e9, 0xb5b32944, 0xdb75092e, 0xc4192623,
        0xad6ea6b0, 0x49a7df7d, 0x9cee60b8, 0x8fedb266,
        0xecaa8c71, 0x699a17ff, 0x5664526c, 0xc2b19ee1,
        0x193602a5, 0x75094c29, 0xa0591340, 0xe4183a3e,
        0x3f54989a, 0x5b429d65, 0x6b8fe4d6, 0x99f73fd6,
        0xa1d29c07, 0xefe830f5, 0x4d2d38e6, 0xf0255dc1,
        0x4cdd2086, 0x8470eb26, 0x6382e9c6, 0x021ecc5e,
        0x09686b3f, 0x3ebaefc9, 0x3c971814, 0x6b6a70a1,
        0x687f3584, 0x52a0e286, 0xb79c5305, 0xaa500737,
        0x3e07841c, 0x7fdeae5c, 0x8e7d44ec, 0x5716f2b8,
        0xb03ada37, 0xf0500c0d, 0xf01c1f04, 0x0200b3ff,
        0xae0cf51a, 0x3cb574b2, 0x25837a58, 0xdc0921bd,
        0xd19113f9, 0x7ca92ff6, 0x94324773, 0x22f54701,
        0x3ae5e581, 0x37c2dadc, 0xc8b57634, 0x9af3dda7,
        0xa9446146, 0x0fd0030e, 0xecc8c73e, 0xa4751e41,
        0xe238cd99, 0x3bea0e2f, 0x3280bba1, 0x183eb331,
        0x4e548b38, 0x4f6db908, 0x6f420d03, 0xf60a04bf,
        0x2cb81290, 0x24977c79, 0x5679b072, 0xbcaf89af,
        0xde9a771f, 0xd9930810, 0xb38bae12, 0xdccf3f2e,
        0x5512721f, 0x2e6b7124, 0x501adde6, 0x9f84cd87,
        0x7a584718, 0x7408da17, 0xbc9f9abc, 0xe94b7d8c,
        0xec7aec3a, 0xdb851dfa, 0x63094366, 0xc464c3d2,
        0xef1c1847, 0x3215d908, 0xdd433b37, 0x24c2ba16,
        0x12a14d43, 0x2a65c451, 0x50940002, 0x133ae4dd,
        0x71dff89e, 0x10314e55, 0x81ac77d6, 0x5f11199b,
        0x043556f1, 0xd7a3c76b, 0x3c11183b, 0x5924a509,
        0xf28fe6ed, 0x97f1fbfa, 0x9ebabf2c, 0x1e153c6e,
        0x86e34570, 0xeae96fb1, 0x860e5e0a, 0x5a3e2ab3,
        0x771fe71c, 0x4e3d06fa, 0x2965dcb9, 0x99e71d0f,
        0x803e89d6, 0x5266c825, 0x2e4cc978, 0x9c10b36a,
        0xc6150eba, 0x94e2ea78, 0xa5fc3c53, 0x1e0a2df4,
        0xf2f74ea7, 0x361d2b3d, 0x1939260f, 0x19c27960,
        0x5223a708, 0xf71312b6, 0xebadfe6e, 0xeac31f66,
        0xe3bc4595, 0xa67bc883, 0xb17f37d1, 0x018cff28,
        0xc332ddef, 0xbe6c5aa5, 0x65582185, 0x68ab9802,
        0xeecea50f, 0xdb2f953b, 0x2aef7dad, 0x5b6e2f84,
        0x1521b628, 0x29076170, 0xecdd4775, 0x619f1510,
        0x13cca830, 0xeb61bd96, 0x0334fe1e, 0xaa0363cf,
        0xb5735c90, 0x4c70a239, 0xd59e9e0b, 0xcbaade14,
        0xeecc86bc, 0x60622ca7, 0x9cab5cab, 0xb2f3846e,
        0x648b1eaf, 0x19bdf0ca, 0xa02369b9, 0x655abb50,
        0x40685a32, 0x3c2ab4b3, 0x319ee9d5, 0xc021b8f7,
        0x9b540b19, 0x875fa099, 0x95f7997e, 0x623d7da8,
        0xf837889a, 0x97e32d77, 0x11ed935f, 0x16681281,
        0x0e358829, 0xc7e61fd6, 0x96dedfa1, 0x7858ba99,
        0x57f584a5, 0x1b227263, 0x9b83c3ff, 0x1ac24696,
        0xcdb30aeb, 0x532e3054, 0x8fd948e4, 0x6dbc3128,
        0x58ebf2ef, 0x34c6ffea, 0xfe28ed61, 0xee7c3c73,
        0x5d4a14d9, 0xe864b7e3, 0x42105d14, 0x203e13e0,
        0x45eee2b6, 0xa3aaabea, 0xdb6c4f15, 0xfacb4fd0,
        0xc742f442, 0xef6abbb5, 0x654f3b1d, 0x41cd2105,
        0xd81e799e, 0x86854dc7, 0xe44b476a, 0x3d816250,
        0xcf62a1f2, 0x5b8d2646, 0xfc8883a0, 0xc1c7b6a3,
        0x7f1524c3, 0x69cb7492, 0x47848a0b, 0x5692b285,
        0x095bbf00, 0xad19489d, 0x1462b174, 0x23820e00,
        0x58428d2a, 0x0c55f5ea, 0x1dadf43e, 0x233f7061,
        0x3372f092, 0x8d937e41, 0xd65fecf1, 0x6c223bdb,
        0x7cde3759, 0xcbee7460, 0x4085f2a7, 0xce77326e,
        0xa6078084, 0x19f8509e, 0xe8efd855, 0x61d99735,
        0xa969a7aa, 0xc50c06c2, 0x5a04abfc, 0x800bcadc,
        0x9e447a2e, 0xc3453484, 0xfdd56705, 0x0e1e9ec9,
        0xdb73dbd3, 0x105588cd, 0x675fda79, 0xe3674340,
        0xc5c43465, 0x713e38d8, 0x3d28f89e, 0xf16dff20,
        0x153e21e7, 0x8fb03d4a, 0xe6e39f2b, 0xdb83adf7,
        0xe93d5a68, 0x948140f7, 0xf64c261c, 0x94692934,
        0x411520f7, 0x7602d4f7, 0xbcf46b2e, 0xd4a20068,
        0xd4082471, 0x3320f46a, 0x43b7d4b7, 0x500061af,
        0x1e39f62e, 0x97244546, 0x14214f74, 0xbf8b8840,
        0x4d95fc1d, 0x96b591af, 0x70f4ddd3, 0x66a02f45,
        0xbfbc09ec, 0x03bd9785, 0x7fac6dd0, 0x31cb8504,
        0x96eb27b3, 0x55fd3941, 0xda2547e6, 0xabca0a9a,
        0x28507825, 0x530429f4, 0x0a2c86da, 0xe9b66dfb,
        0x68dc1462, 0xd7486900, 0x680ec0a4, 0x27a18dee,
        0x4f3ffea2, 0xe887ad8c, 0xb58ce006, 0x7af4d6b6,
        0xaace1e7c, 0xd3375fec, 0xce78a399, 0x406b2a42,
        0x20fe9e35, 0xd9f385b9, 0xee39d7ab, 0x3b124e8b,
        0x1dc9faf7, 0x4b6d1856, 0x26a36631, 0xeae397b2,
        0x3a6efa74, 0xdd5b4332, 0x6841e7f7, 0xca7820fb,
        0xfb0af54e, 0xd8feb397, 0x454056ac, 0xba489527,
        0x55533a3a, 0x20838d87, 0xfe6ba9b7, 0xd096954b,
        0x55a867bc, 0xa1159a58, 0xcca92963, 0x99e1db33,
        0xa62a4a56, 0x3f3125f9, 0x5ef47e1c, 0x9029317c,
        0xfdf8e802, 0x04272f70, 0x80bb155c, 0x05282ce3,
        0x95c11548, 0xe4c66d22, 0x48c1133f, 0xc70f86dc,
        0x07f9c9ee, 0x41041f0f, 0x404779a4, 0x5d886e17,
        0x325f51eb, 0xd59bc0d1, 0xf2bcc18f, 0x41113564,
        0x257b7834, 0x602a9c60, 0xdff8e8a3, 0x1f636c1b,
        0x0e12b4c2, 0x02e1329e, 0xaf664fd1, 0xcad18115,
        0x6b2395e0, 0x333e92e1, 0x3b240b62, 0xeebeb922,
        0x85b2a20e, 0xe6ba0d99, 0xde720c8c, 0x2da2f728,
        0xd0127845, 0x95b794fd, 0x647d0862, 0xe7ccf5f0,
        0x5449a36f, 0x877d48fa, 0xc39dfd27, 0xf33e8d1e,
        0x0a476341, 0x992eff74, 0x3a6f6eab, 0xf4f8fd37,
        0xa812dc60, 0xa1ebddf8, 0x991be14c, 0xdb6e6b0d,
        0xc67b5510, 0x6d672c37, 0x2765d43b, 0xdcd0e804,
        0xf1290dc7, 0xcc00ffa3, 0xb5390f92, 0x690fed0b,
        0x667b9ffb, 0xcedb7d9c, 0xa091cf0b, 0xd9155ea3,
        0xbb132f88, 0x515bad24, 0x7b9479bf, 0x763bd6eb,
        0x37392eb3, 0xcc115979, 0x8026e297, 0xf42e312d,
        0x6842ada7, 0xc66a2b3b, 0x12754ccc, 0x782ef11c,
        0x6a124237, 0xb79251e7, 0x06a1bbe6, 0x4bfb6350,
        0x1a6b1018, 0x11caedfa, 0x3d25bdd8, 0xe2e1c3c9,
        0x44421659, 0x0a121386, 0xd90cec6e, 0xd5abea2a,
        0x64af674e, 0xda86a85f, 0xbebfe988, 0x64e4c3fe,
        0x9dbc8057, 0xf0f7c086, 0x60787bf8, 0x6003604d,
        0xd1fd8346, 0xf6381fb0, 0x7745ae04, 0xd736fccc,
        0x83426b33, 0xf01eab71, 0xb0804187, 0x3c005e5f,
        0x77a057be, 0xbde8ae24, 0x55464299, 0xbf582e61,
        0x4e58f48f, 0xf2ddfda2, 0xf474ef38, 0x8789bdc2,
        0x5366f9c3, 0xc8b38e74, 0xb475f255, 0x46fcd9b9,
        0x7aeb2661, 0x8b1ddf84, 0x846a0e79, 0x915f95e2,
        0x466e598e, 0x20b45770, 0x8cd55591, 0xc902de4c,
        0xb90bace1, 0xbb8205d0, 0x11a86248, 0x7574a99e,
        0xb77f19b6, 0xe0a9dc09, 0x662d09a1, 0xc4324633,
        0xe85a1f02, 0x09f0be8c, 0x4a99a025, 0x1d6efe10,
        0x1ab93d1d, 0x0ba5a4df, 0xa186f20f, 0x2868f169,
        0xdcb7da83, 0x573906fe, 0xa1e2ce9b, 0x4fcd7f52,
        0x50115e01, 0xa70683fa, 0xa002b5c4, 0x0de6d027,
        0x9af88c27, 0x773f8641, 0xc3604c06, 0x61a806b5,
        0xf0177a28, 0xc0f586e0, 0x006058aa, 0x30dc7d62,
        0x11e69ed7, 0x2338ea63, 0x53c2dd94, 0xc2c21634,
        0xbbcbee56, 0x90bcb6de, 0xebfc7da1, 0xce591d76,
        0x6f05e409, 0x4b7c0188, 0x39720a3d, 0x7c927c24,
        0x86e3725f, 0x724d9db9, 0x1ac15bb4, 0xd39eb8fc,
        0xed545578, 0x08fca5b5, 0xd83d7cd3, 0x4dad0fc4,
        0x1e50ef5e, 0xb161e6f8, 0xa28514d9, 0x6c51133c,
        0x6fd5c7e7, 0x56e14ec4, 0x362abfce, 0xddc6c837,
        0xd79a3234, 0x92638212, 0x670efa8e, 0x406000e0,
        0x3a39ce37, 0xd3faf5cf, 0xabc27737, 0x5ac52d1b,
        0x5cb0679e, 0x4fa33742, 0xd3822740, 0x99bc9bbe,
        0xd5118e9d, 0xbf0f7315, 0xd62d1c7e, 0xc700c47b,
        0xb78c1b6b, 0x21a19045, 0xb26eb1be, 0x6a366eb4,
        0x5748ab2f, 0xbc946e79, 0xc6a376d2, 0x6549c2c8,
        0x530ff8ee, 0x468dde7d, 0xd5730a1d, 0x4cd04dc6,
        0x2939bbdb, 0xa9ba4650, 0xac9526e8, 0xbe5ee304,
        0xa1fad5f0, 0x6a2d519a, 0x63ef8ce2, 0x9a86ee22,
        0xc089c2b8, 0x43242ef6, 0xa51e03aa, 0x9cf2d0a4,
        0x83c061ba, 0x9be96a4d, 0x8fe51550, 0xba645bd6,
        0x2826a2f9, 0xa73a3ae1, 0x4ba99586, 0xef5562e9,
        0xc72fefd3, 0xf752f7da, 0x3f046f69, 0x77fa0a59,
        0x80e4a915, 0x87b08601, 0x9b09e6ad, 0x3b3ee593,
        0xe990fd5a, 0x9e34d797, 0x2cf0b7d9, 0x022b8b51,
        0x96d5ac3a, 0x017da67d, 0xd1cf3ed6, 0x7c7d2d28,
        0x1f9f25cf, 0xadf2b89b, 0x5ad6b472, 0x5a88f54c,
        0xe029ac71, 0xe019a5e6, 0x47b0acfd, 0xed93fa9b,
        0xe8d3c48d, 0x283b57cc, 0xf8d56629, 0x79132e28,
        0x785f0191, 0xed756055, 0xf7960e44, 0xe3d35e8c,
        0x15056dd4, 0x88f46dba, 0x03a16125, 0x0564f0bd,
        0xc3eb9e15, 0x3c9057a2, 0x97271aec, 0xa93a072a,
        0x1b3f6d9b, 0x1e6321f5, 0xf59c66fb, 0x26dcf319,
        0x7533d928, 0xb155fdf5, 0x03563482, 0x8aba3cbb,
        0x28517711, 0xc20ad9f8, 0xabcc5167, 0xccad925f,
        0x4de81751, 0x3830dc8e, 0x379d5862, 0x9320f991,
        0xea7a90c2, 0xfb3e7bce, 0x5121ce64, 0x774fbe32,
        0xa8b6e37e, 0xc3293d46, 0x48de5369, 0x6413e680,
        0xa2ae0810, 0xdd6db224, 0x69852dfd, 0x09072166,
        0xb39a460a, 0x6445c0dd, 0x586cdecf, 0x1c20c8ae,
        0x5bbef7dd, 0x1b588d40, 0xccd2017f, 0x6bb4e3bb,
        0xdda26a7e, 0x3a59ff45, 0x3e350a44, 0xbcb4cdd5,
        0x72eacea8, 0xfa6484bb, 0x8d6612ae, 0xbf3c6f47,
        0xd29be463, 0x542f5d9e, 0xaec2771b, 0xf64e6370,
        0x740e0d8d, 0xe75b1357, 0xf8721671, 0xaf537d5d,
        0x4040cb08, 0x4eb4e2cc, 0x34d2466a, 0x0115af84,
        0xe1b00428, 0x95983a1d, 0x06b89fb4, 0xce6ea048,
        0x6f3f3b82, 0x3520ab82, 0x011a1d4b, 0x277227f8,
        0x611560b1, 0xe7933fdc, 0xbb3a792b, 0x344525bd,
        0xa08839e1, 0x51ce794b, 0x2f32c9b7, 0xa01fbac9,
        0xe01cc87e, 0xbcc7d1f6, 0xcf0111c3, 0xa1e8aac7,
        0x1a908749, 0xd44fbd9a, 0xd0dadecb, 0xd50ada38,
        0x0339c32a, 0xc6913667, 0x8df9317c, 0xe0b12b4f,
        0xf79e59b7, 0x43f5bb3a, 0xf2d519ff, 0x27d9459c,
        0xbf97222c, 0x15e6fc2a, 0x0f91fc71, 0x9b941525,
        0xfae59361, 0xceb69ceb, 0xc2a86459, 0x12baa8d1,
        0xb6c1075e, 0xe3056a0c, 0x10d25065, 0xcb03a442,
        0xe0ec6e0e, 0x1698db3b, 0x4c98a0be, 0x3278e964,
        0x9f1f9532, 0xe0d392df, 0xd3a0342b, 0x8971f21e,
        0x1b0a7441, 0x4ba3348c, 0xc5be7120, 0xc37632d8,
        0xdf359f8d, 0x9b992f2e, 0xe60b6f47, 0x0fe3f11d,
        0xe54cda54, 0x1edad891, 0xce6279cf, 0xcd3e7e6f,
        0x1618b166, 0xfd2c1d05, 0x848fd2c5, 0xf6fb2299,
        0xf523f357, 0xa6327623, 0x93a83531, 0x56cccd02,
        0xacf08162, 0x5a75ebb5, 0x6e163697, 0x88d273cc,
        0xde966292, 0x81b949d0, 0x4c50901b, 0x71c65614,
        0xe6c6c7bd, 0x327a140a, 0x45e1d006, 0xc3f27b9a,
        0xc9aa53fd, 0x62a80f00, 0xbb25bfe2, 0x35bdd2f6,
        0x71126905, 0xb2040222, 0xb6cbcf7c, 0xcd769c2b,
        0x53113ec0, 0x1640e3d3, 0x38abbd60, 0x2547adf0,
        0xba38209c, 0xf746ce76, 0x77afa1c5, 0x20756060,
        0x85cbfe4e, 0x8ae88dd8, 0x7aaaf9b0, 0x4cf9aa7e,
        0x1948c25c, 0x02fb8a8c, 0x01c36ae4, 0xd6ebe1f9,
        0x90d4f869, 0xa65cdea0, 0x3f09252d, 0xc208e69f,
        0xb74e6132, 0xce77e25b, 0x578fdfe3, 0x3ac372e6,
    };

#define S(x,i) (context.sboxes_[i][x.w_.byte##i##_])
#define bf_F(x) (((S(x,0) + S(x,1)) ^ S(x,2)) + S(x,3))
#define ROUND(a,b,n) (a.dword_ ^= bf_F(b) ^ context.parray_[n])

}

    void BlowFish::encrypt(const BlowFishContext& context, u8* cipher, const u8* data, s32 dataLength)
    {
        LASSERT(NULL != cipher);
        LASSERT(NULL != data);
        LASSERT(PKCS5::checkLength(dataLength, BlockSizeInBytes));

        const u32 halfBlockSize = BlockSizeInBytes>>1;

        if(cipher == data){
            u8* end = cipher + dataLength;

            while(cipher<end){
                encrypt(context, (u32*)cipher, (u32*)(cipher+halfBlockSize));
                cipher += BlockSizeInBytes;
            }

        }else{
            u8* end = cipher + dataLength;

            while(cipher<end){
                for(s32 i=0; i<8; ++i){
                    cipher[i] = data[i];
                }
                encrypt(context, (u32*)cipher, (u32*)(cipher+halfBlockSize));
                cipher += BlockSizeInBytes;
                data += BlockSizeInBytes;
            }
        }
    }

    void BlowFish::decrypt(const BlowFishContext& context, u8* data, const u8* cipher, s32 cipherLength)
    {
        LASSERT(NULL != data);
        LASSERT(NULL != cipher);
        LASSERT(PKCS5::checkLength(cipherLength, BlockSizeInBytes));

        const u32 halfBlockSize = BlockSizeInBytes>>1;

        if(data == cipher){
            u8* end = data + cipherLength;

            while(data<end){
                decrypt(context, (u32*)data, (u32*)(data+halfBlockSize));
                data += BlockSizeInBytes;
            }

        }else{
            u8* end = data + cipherLength;

            while(data<end){
                for(s32 i=0; i<8; ++i){
                    data[i] = cipher[i];
                }
                decrypt(context, (u32*)data, (u32*)(data+halfBlockSize));
                data += BlockSizeInBytes;
                cipher += BlockSizeInBytes;
            }
        }
    }

    void BlowFish::encrypt(const BlowFishContext& context, u32* xl, u32* xr)
    {
        aword Xl, Xr;

        Xl.dword_ = *xl ;
        Xr.dword_ = *xr ;

        Xl.dword_ ^= context.parray_[0];
        ROUND(Xr, Xl, 1);  ROUND(Xl, Xr, 2);
        ROUND(Xr, Xl, 3);  ROUND(Xl, Xr, 4);
        ROUND(Xr, Xl, 5);  ROUND(Xl, Xr, 6);
        ROUND(Xr, Xl, 7);  ROUND(Xl, Xr, 8);
        ROUND(Xr, Xl, 9);  ROUND(Xl, Xr, 10);
        ROUND(Xr, Xl, 11); ROUND(Xl, Xr, 12);
        ROUND(Xr, Xl, 13); ROUND(Xl, Xr, 14);
        ROUND(Xr, Xl, 15); ROUND(Xl, Xr, 16);
        Xr.dword_ ^= context.parray_[17];

        *xr = Xl.dword_ ;
        *xl = Xr.dword_ ;
    }

    void BlowFish::decrypt(const BlowFishContext& context, u32* xl, u32* xr)
    {
        aword Xl, Xr;
        Xl.dword_ = *xl ;
        Xr.dword_ = *xr ;

        Xl.dword_ ^= context.parray_[17] ;
        ROUND(Xr, Xl, 16); ROUND(Xl, Xr, 15);
        ROUND(Xr, Xl, 14); ROUND(Xl, Xr, 13);
        ROUND(Xr, Xl, 12); ROUND(Xl, Xr, 11);
        ROUND(Xr, Xl, 10); ROUND(Xl, Xr, 9);
        ROUND(Xr, Xl, 8);  ROUND(Xl, Xr, 7);
        ROUND(Xr, Xl, 6);  ROUND(Xl, Xr, 5);
        ROUND(Xr, Xl, 4);  ROUND(Xl, Xr, 3);
        ROUND(Xr, Xl, 2);  ROUND(Xl, Xr, 1);
        Xr.dword_ ^= context.parray_[0];

        *xl = Xr.dword_;
        *xr = Xl.dword_;
    }

    void BlowFishContext::initialize(u8* key, s32 length)
    {
        BlowFish::aword temp;
        for(s32 i=0; i<18; ++i){
            parray_[i] = bf_P[i];
        }
        for(s32 i=0; i<4; ++i){
            for(s32 j=0; j<256; ++j){
                sboxes_[i][j] = bf_S[i][j];
            }
        }


        s32 index = 0;
        for(s32 i=0; i<BlowFish::NPass+2; ++i){
            temp.dword_ = 0;
            temp.w_.byte0_ = key[index];
            temp.w_.byte1_ = key[(index+1)%length];
            temp.w_.byte2_ = key[(index+2)%length];
            temp.w_.byte3_ = key[(index+3)%length];
            u32 data = temp.dword_;
            parray_[i] ^= data;
            index = (index+4) % length;
        }

        u32 datal = 0;
        u32 datar = 0;
        for(s32 i=0; i<BlowFish::NPass+2; i+=2){
            BlowFish::encrypt(*this, &datal, &datar);
            parray_[i] = datal;
            parray_[i+1] = datar;
        }
        for(s32 i=0; i<4; ++i){
            for(s32 j=0; j<256; j+=2){
                BlowFish::encrypt(*this, &datal, &datar);
                sboxes_[i][j] = datal;
                sboxes_[i][j+1] = datar;
            }
        }
    }
}
