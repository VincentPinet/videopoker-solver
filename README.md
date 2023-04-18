# videopoker-solver

## Methodology

A simple approach to solve the game would be: for each of the 5 cards starting hand, find the maximum among the 32 ways of playing it by calculating the expected return of possible outcomes in the respective leftover deck.  

$${52 \choose 5}\sum_{k=0}^{5}{5\choose k}{47\choose 5-k} = 6,754,593,081,600$$

Depending on how efficient our combo detection is, this could take hours.. We can't afford that and will need some shortcuts.

## Equivalence classes

Suits are always good candidate for symmetry even though we do have flushes in video poker, as opposed to blackjack where they can be completely cut off, we still can identify that (Beside the obvious fact that holding everything here is optimal):

A♠️ K♠️ Q♠️ J♠️ T♠️  
A♥️ K♥️ Q♥️ J♥️ T♥️  
A♦️ K♦️ Q♦️ J♦️ T♦️  
A♣️ K♣️ Q♣️ J♣️ T♣️  

The payout of one suit is no different that any other. Suits are interchangeable. This means that we can choose one canonical way of representing this group and count it 4 times.

Once we start mixing suits there will be equivalence classes of sizes 4, 12 and 24. This reduces our initial starting hand from ${52\choose 5} = 2,598,960$ down to $134,459$.

## Discard effect

Let's consider the following holdings and their outcome distribution when hunting for royal straight flush.

| Holding | Royal | Straight Flush | Flush | Straight | Jacks or Better | Nothing | Total | EV |
|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|
| K♥️ Q♥️ J♥️ T♥️ ~~2♠️~~ | 1 | 1 | 7 | 6 | 9 | ***23*** | 47 | 19.680 |
| K♥️ Q♥️ J♥️ T♥️ ~~J♠️~~ | 1 | 1 | 7 | 6 | ***8*** | 24 | 47 | 19.659 |
| K♥️ Q♥️ J♥️ T♥️ ~~9♠️~~ | 1 | 1 | 7 | ***5*** | 9 | 24 | 47 | 19.595 |
| K♥️ Q♥️ J♥️ T♥️ ~~2♥️~~ | 1 | 1 | ***6*** | 6 | 9 | 24 | 47 | 19.553 |

At first it might seems surprising why the worst holding actually has the best expected return.
But when we think about it, breaking a *good* combination in order to chase that A♥️ worsen the quality of the fallback combo when we don't hit.  
But more importantly, even though the remaining 47 cards of the deck, and therefore the expected return, are not exactly the same in each of those holdings. The intuition is that it's close enough that there must be a way to jump from one to another without having the go through the whole deck again to compute it.

Now let's consider this 4 cards holding with, subsequently, a deck of 48 cards left:

| Holding | Royal | Straight Flush | Flush | Straight | Jacks or Better | Nothing | Total |
|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|
| K♥️ Q♥️ J♥️ T♥️ __ | 1 | 1 | 7 | 6 | 9 | 24 | ***48*** |

In order to compute any 5th card discard, we can adjust this distribution by subtracting the penalty engendered by whatever combo this card was making.

This discard effect is a little more subtle to compute when discarding more than one card. We are now subtracting multiple distributions instead of just one combo and are subject to double counting, so we need to alternate between subtracting and adding back.

Complexity wise we basically converted the ${47\choose k}$ to a fixed cost for precomputing all distributions and are left with a small merging coefficient for each initial starting hand in the size of payout categories.

## Let the bit hacking begin

We need something to represent a collection of cards. Luckily for us video poker is played with a 52 unique cards. Meaning we can fit all the information inside a 64 bits integer, here is the layout I landed on:

| \ |K |Q |J |T |9 |8 |7 |6 |5 |4 |3 |2 |A |
|---|--|--|--|--|--|--|--|--|--|--|--|--|--|
| ♠️ |60|59|58|57|56|55|54|53|52|51|50|49|48|
| ♥️ |44|43|42|41|40|39|38|37|36|35|34|33|32|
| ♦️ |28|27|26|25|24|23|22|21|20|19|18|17|16|
| ♣️ |12|11|10| 9| 8| 7| 6| 5| 4| 3| 2| 1| 0|

With the following bits being unused 63, 62, 61, 47, 46, 45, 31, 30, 29, 15, 14, 13.  
I thought it might ease the compiler to align the suits on 16 bits since we are going to fold them onto each other with bitwise *and* to identify a four of a kind for instance. Most of the time *or* and *xor* in addition of an abundant use of the *popcnt* instruction for various pairs detection, even a little of *ctz* and *clz* for straights.

Another thing we are going to want is an easy way to iterate over the n choose k distinct subsets of our bitcards, whether it's for the different ways of holding a hand or the possible outcomes of deck.  
For algorithmic readability purpose I wanted an aesthetic user api and it was a good opportunity to have fun the the c++20 std::ranges.

The secret ingredient to do so with minimal memory and complexity footprint is [**parallel bits deposit** instruction](https://en.wikipedia.org/wiki/X86_Bit_manipulation_instruction_set#Parallel_bit_deposit_and_extract).
The idea is that our bitcards is going to act as the selector mask, we then start with k contiguous low-order bits and advance them with a [lexicographically next bit permutation](https://graphics.stanford.edu/~seander/bithacks.html#NextBitPermutation) to iterate.

The latest constexpr [*\<bit\>*](https://en.cppreference.com/w/cpp/header/bit) header doesn't have parallel bits deposit in it, but fortunately for me GCC has this life saving undocumented feature that `__builtin_ia32_pdep_di` function is constexpr and funny enough the `_pdep_u64` wrapper from *immintrin.h* is not. Well, I guess the project is not so portable anymore.

## The `constexpr` journey

One of the self inflicted challenge of this project was to make everything constexpr. If you intentional code things in a non-configurable way with an hardcoded variant, every single information should be knowable at compile time.  
But I underestimated the inefficiency of the compiler optimization compare to plain runtime.  
For instance the following snippet of code, naive solver approach that return the expected value of a fixed hand under optimal play  which fully lives in a constexpr context:

```c++
static constexpr double solve(const vps::bitcards& hand) {
    vps::bitcards deck = vps::bitcards::full_deck() - hand;
    double best = vps::payout(hand.eval());
    for (int k = 1; k <= 5; k++) {
        for (const auto& discard : hand.choose(k)) {
            double curr = 0;
            for (const auto& outcome : deck.choose(k)) {
                vps::bitcards tmp = hand - discard + outcome;
                curr += vps::payout(tmp.eval());
            }
            best = std::max(best, curr / nck(deck.size(), k));
        }
    }
    return best;
}

int main() {
    static constexpr vps::bitcards hand{"Kh Qh Jh Th 2s"};
    static constexpr double ans = solve(hand);
    return static_cast<int>(ans);
}
```

Would take about 20 ms without the constexpr specifier; but somehow doesn't compile at all after more than 30 minutes even with 16 threads while using 40 Go of swap..  
If solving ONE hand wasn't trivially doable, the whole game is probably impossible. But because I am not willing to go down without at least a small victory; if we unfairly remove from consideration throwing away all five (and after some tweaking with the `-fconstexpr-*` compile options), the resulting binary looks like this:

```nasm
    main:
        mov     eax, 19
        ret
```

Useless but neat!

## Solver

The main dream of the project being dead in the water at this stage; For the precomputing storage, I decided not to bother to much and abuse *unordered_map* for simplicity and code readability, which is not constexpr compliant. You can definitely optimize this a lot with statically allocated array and a smart way to map hand to indices. Or even storing on disk the distributions, also worth noting it's not payout table dependant so you can still do runtime configuration and benefit from this. The program runs in about 6 seconds, it's fine by me for now.

## Results

(make sure you are NOT using Clang or MSVC)

```shell
cmake -B build -DCMAKE_CXX_COMPILER:FILEPATH=/usr/bin/g++
cmake --build build
./build/app/videopoker-solver-app
```

The 9/6 Jacks or Better variant has been wildly studied over the years and the $0.995439$ output of my program can be cross-checked easily.  
