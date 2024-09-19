#include <iostream>
#include <cstdint>
#include <unordered_set>
#include <random>
#include <string>

using namespace std;

//Type of unsigned int64 that the generator uses (for easy access)
typedef uint64_t uint64;

//Base class for custom RNG implementations
class CustomRNG
{
public:
  explicit CustomRNG(uint64 seed) : _seed(seed) {};

  virtual uint64 generate() = 0;
  virtual uint64 generateRange(uint64 min, uint64 max) = 0;

protected:
  uint64 _seed = 0;
};

//Reference: https://rosettacode.org/wiki/Pseudo-random_numbers/Splitmix64#:~:text=Splitmix64%20is%20the%20default%20pseudo,for%20many%20random%20number%20needs.
class Splitmix64 : CustomRNG
{
public:
  explicit Splitmix64(uint64 seed) : CustomRNG(seed) {};

  virtual uint64 generate() override;
  virtual uint64 generateRange(uint64 min, uint64 max) override;
};

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


class Xoshiro256Star : CustomRNG
{
public:
  explicit Xoshiro256Star(uint64 seed) : CustomRNG(seed) {};

  virtual uint64 generate() override;
  virtual uint64 generateRange(uint64 min, uint64 max) override;

private:
  uint64 _s[4];
};

//https://prng.di.unimi.it/xoshiro256starstar.c
uint64 Xoshiro256Star::generate()
{
  //Rotate
  _seed = ((_s[1] * 5) << 7) | ((_s[1] * 5) >> 57);
  _seed *= 9;

  uint64 oldS1 = _s[1] << 17;

  //Xor
  _s[2] ^= _s[0];
  _s[3] ^= _s[1];
  _s[1] ^= _s[2];
  _s[0] ^= _s[3];

  _s[2] ^= oldS1;

  //Rotate
  _s[3] = (_s[3] << 45) | (_s[3] >> 19);

  return _seed;
}

uint64 Xoshiro256Star::generateRange(uint64 min, uint64 max)
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

struct TestResult
{
  string name;
  float avgWarming = 0.f;
  float avgPeriodic = 0.f;
};

template<typename T, typename = std::enable_if_t<std::is_base_of_v<CustomRNG, T>>>
TestResult runTests(uniform_int_distribution<uint64> distribution, mt19937 rng, uint64 iterations, uint64 rangeMin, uint64 rangeMax, string name)
{
  //Values to calculate averages from at the end
  int warmings = 0;
  int periodics = 0;

  for (int i = 0; i < iterations; i++)
  {
    //Initialize generator (with another RNG! :) )
    T generator(distribution(rng));

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
          int warming =  currentState.iteration;
          int periodic = iter - currentState.iteration;

          warmings += warming;
          periodics += periodic;

          printf("Warming phase = %i, periodic phase = %i (in range %i-%i)\n", warming, periodic, rangeMin, rangeMax);
          cont = false;
          break;
        }
      }

      stateSet.insert(nextState);
    }

    //Clear set for next iteration
    stateSet.clear();
  }

  TestResult result;
  result.avgWarming = float(warmings)/float(iterations);
  result.avgPeriodic = float(periodics)/float(iterations);
  result.name = name;

  printf("Avg. warming phase = %f, avg. periodic phase = %f, over %i iterations\n", result.avgWarming, result.avgPeriodic, iterations);

  return result;
}

int main()
{
  uint64 iterations, rangeMin, rangeMax;

  cout << "Enter iterations, range min, range max\n";
  cin >> iterations >> rangeMin >> rangeMax;

  //Use RNG to initialize the RNG!
  random_device device;
  mt19937 rng(device());
  uniform_int_distribution<uint64> distribution(0, UINT64_MAX);

  vector<TestResult> results;

  printf("Running tests for Splitmix64\n");
  results.push_back(runTests<Splitmix64>(distribution, rng, iterations, rangeMin, rangeMax, "Splitmix64"));

  printf("Running tests for Xoshiro256*\n");
  results.push_back(runTests<Xoshiro256Star>(distribution, rng, iterations, rangeMin, rangeMax, "Xoshiro256*"));

  printf("Overall test results:\nName\tAvg. warming\tAvg. periodic\n");
  for (const TestResult& result : results)
  {
    printf("%s\t%f\t%f\n", result.name.c_str(), result.avgWarming, result.avgPeriodic);
  }

  return 0;
}