#include <iostream>
#include <cstdint>
#include <unordered_set>
#include <random>

using namespace std;

//Type of unsigned int64 that the generator uses (for easy access)
typedef uint64_t uint64;

//Reference: https://rosettacode.org/wiki/Pseudo-random_numbers/Splitmix64#:~:text=Splitmix64%20is%20the%20default%20pseudo,for%20many%20random%20number%20needs.
class Splitmix64
{
public:
  Splitmix64(uint64 seed);

  uint64 generate();
  uint64 generateRange(uint64 min, uint64 max);

private:
  uint64 _seed = 0;
};

Splitmix64::Splitmix64(uint64 seed) : _seed (seed) {}

uint64 Splitmix64::generate()
{
  //Increment seed
  _seed += 0x9e3779b97f4a7c15;

  //Store seed in variable that will be outputted
  uint64 result = _seed;

  //xorshift then multiply by constant (step 1)
  result ^= result >> 30;
  result *= 0xbf58476d1ce4e5b9;

  //xorshift then multiply by constant (step 2)
  result ^= result >> 27;
  result *= 0x94d049bb133111eb;

  //final xorshift
  result ^= result >> 31;

  return result;
}

uint64 Splitmix64::generateRange(uint64 min, uint64 max)
{
  return generate() % (max - min) + min;
}

struct genState
{
  uint64 state;
  int iteration;

  bool operator==(const genState &other) const
  {
    return state == other.state;
  }
};

auto hashFunction = [](const genState &state)
{
  //Knuth's Multiplicative Method https://gist.github.com/badboy/6267743
  return (state.state * 2654435761) >> 32;
};

unordered_set<genState, decltype(hashFunction)> stateSet;

int main()
{
  uint64 iterations, rangeMin, rangeMax;

  cout << "Enter iterations, range min, range max\n";
  cin >> iterations >> rangeMin >> rangeMax;

  //Use RNG to initialize the RNG!
  random_device device;
  mt19937 rng(device());
  uniform_int_distribution<uint64> distribution(0, UINT64_MAX);

  //Values to calculate averages from at the end
  int transients = 0;
  int currents = 0;

  for (int i = 0; i < iterations; i++)
  {
    //Initialize generator (with another RNG! :) )
    Splitmix64 generator(distribution(rng));

    printf("Iteration %i: ", i);

    int iter = 0;

    //Scary!
    bool cont = true;
    while (cont)
    {
      //Generate next state
      genState nextState{generator.generateRange(rangeMin, rangeMax), iter++};

      for (const genState& currentState : stateSet)
      {
        if (currentState == nextState)
        {
          int transient =  currentState.iteration;
          int current = iter - currentState.iteration;

          transients += transient;
          currents += current;

          printf("Transient state = %i, current state = %i (in range %i-%i)\n", transient, current, rangeMin, rangeMax);
          cont = false;
          break;
        }
      }

      stateSet.insert(nextState);
    }

    //Clear set for next iteration
    stateSet.clear();
  }

  printf("Avg. transient state = %f, avg. current state = %f, over %i iterations\n", float(transients)/float(iterations), float(currents)/float(iterations), iterations);

  return 0;
}