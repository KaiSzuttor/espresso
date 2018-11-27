#include <random>
#include <algorithm>
#include <benchmark/benchmark.h>

#include "grid_based_algorithms/lb-d3q19.hpp"

#include <boost/range/numeric.hpp>
#include <boost/numeric/ublas/matrix_sparse.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/operation.hpp>
#include <blaze/math/StaticMatrix.h>
#include <blaze/math/StaticVector.h>
#include <blaze/math/CompressedMatrix.h>

#define BOOST_UBLAS_NDEBUG

std::array<double, 19> lb_calc_m_from_n(const std::array<double, 19> &n) {
  std::array<double, 19> m;
  for (int i = 0; i < 19; i++) {
    m[i] = boost::inner_product(::D3Q19::e_ki[i], n, 0.0);
  }
  return m;
}

std::array<double, 19> lb_calc_m_from_n_unrolled(const std::array<double, 19> &n) {
  std::array<double, 19> m;
  for (int i = 0; i < 19; i++) {
    double tmp=0.0;
    for (int j=0; j<19; j++) {
      tmp += ::D3Q19::e_ki[i][j] * n[j];
    }
    m[i] = tmp;
  }
  return m;
}


void lb_calc_m_from_n_old(const std::array<double, 19>& populations, double *mode) {
  double n0, n1p, n1m, n2p, n2m, n3p, n3m, n4p, n4m, n5p, n5m, n6p, n6m, n7p,
      n7m, n8p, n8m, n9p, n9m;

  n0 = populations[0];
  n1p = populations[1] + populations[2];
  n1m = populations[1] - populations[2];
  n2p = populations[3] + populations[4];
  n2m = populations[3] - populations[4];
  n3p = populations[5] + populations[6];
  n3m = populations[5] - populations[6];
  n4p = populations[7] + populations[8];
  n4m = populations[7] - populations[8];
  n5p = populations[9] + populations[10];
  n5m = populations[9] - populations[10];
  n6p = populations[11] + populations[12];
  n6m = populations[11] - populations[12];
  n7p = populations[13] + populations[14];
  n7m = populations[13] - populations[14];
  n8p = populations[15] + populations[16];
  n8m = populations[15] - populations[16];
  n9p = populations[17] + populations[18];
  n9m = populations[17] - populations[18];

  /* mass mode */
  mode[0] = n0 + n1p + n2p + n3p + n4p + n5p + n6p + n7p + n8p + n9p;

  /* momentum modes */
  mode[1] = n1m + n4m + n5m + n6m + n7m;
  mode[2] = n2m + n4m - n5m + n8m + n9m;
  mode[3] = n3m + n6m - n7m + n8m - n9m;

  /* stress modes */
  mode[4] = -n0 + n4p + n5p + n6p + n7p + n8p + n9p;
  mode[5] = n1p - n2p + n6p + n7p - n8p - n9p;
  mode[6] = n1p + n2p - n6p - n7p - n8p - n9p - 2. * (n3p - n4p - n5p);
  mode[7] = n4p - n5p;
  mode[8] = n6p - n7p;
  mode[9] = n8p - n9p;

  /* kinetic modes */
  mode[10] = -2. * n1m + n4m + n5m + n6m + n7m;
  mode[11] = -2. * n2m + n4m - n5m + n8m + n9m;
  mode[12] = -2. * n3m + n6m - n7m + n8m - n9m;
  mode[13] = n4m + n5m - n6m - n7m;
  mode[14] = n4m - n5m - n8m - n9m;
  mode[15] = n6m - n7m - n8m + n9m;
  mode[16] = n0 + n4p + n5p + n6p + n7p + n8p + n9p - 2. * (n1p + n2p + n3p);
  mode[17] = -n1p + n2p + n6p + n7p - n8p - n9p;
  mode[18] = -n1p - n2p - n6p - n7p - n8p - n9p + 2. * (n3p + n4p + n5p);
}

void lb_calc_m_from_n_boost(const boost::numeric::ublas::compressed_matrix<double>& m, const boost::numeric::ublas::vector<double> &n, boost::numeric::ublas::vector<double>& v) {
  boost::numeric::ublas::axpy_prod(m, n, v, false);
}

void lb_calc_m_from_n_blaze(const blaze::StaticMatrix<double,19UL,19UL>& m, const blaze::StaticVector<double, 19UL>& n, blaze::StaticVector<double, 19UL>& v) {
  v=m*n;
}

std::mt19937 mt(10);
std::uniform_real_distribution<double> dist(1.0, 10.0);

static void BM_LB_Modes_Calculation_New(benchmark::State& state) {
  for (auto _ : state) {
    state.PauseTiming();
    std::array<double, 19> n;
    std::generate(n.begin(), n.end(), [](){return dist(mt);});
    state.ResumeTiming();
    auto const modes = lb_calc_m_from_n(n);
  }
}

BENCHMARK(BM_LB_Modes_Calculation_New);

static void BM_LB_Modes_Calculation_Unrolled(benchmark::State& state) {
  for (auto _ : state) {
    state.PauseTiming();
    std::array<double, 19> n;
    std::generate(n.begin(), n.end(), [](){return dist(mt);});
    state.ResumeTiming();
    auto const modes = lb_calc_m_from_n_unrolled(n);
  }
}

BENCHMARK(BM_LB_Modes_Calculation_Unrolled);

static void BM_LB_Modes_Calculation_Old(benchmark::State& state) {
  for (auto _ : state) {
    state.PauseTiming();
    std::array<double, 19> n;
    std::array<double, 19> m;
    std::generate(n.begin(), n.end(), [](){return dist(mt);});
    state.ResumeTiming();
    lb_calc_m_from_n_old(n, m.data());
  }
}

BENCHMARK(BM_LB_Modes_Calculation_Old);

static void BM_LB_Modes_Calculation_Boost(benchmark::State& state) {
  for (auto _ : state) {
    state.PauseTiming();
    boost::numeric::ublas::compressed_matrix<double> m(19, 19);
    for (unsigned i=0; i<19; i++) {
      for (unsigned j=0; j<19; j++) {
        if (std::abs(::D3Q19::e_ki[i][j]) > std::numeric_limits<double>::epsilon())
          m(i,j) = ::D3Q19::e_ki[i][j];
      }
    }
    boost::numeric::ublas::vector<double> n(19);
    boost::numeric::ublas::vector<double> res(19);
    std::generate(n.begin(), n.end(), [](){return dist(mt);});
    state.ResumeTiming();
    lb_calc_m_from_n_boost(m, n, res);
  }
}

BENCHMARK(BM_LB_Modes_Calculation_Boost);

static void BM_LB_Modes_Calculation_blaze(benchmark::State& state) {
  blaze::StaticMatrix<double, 19UL, 19UL> m;
  for (auto _ : state) {
    state.PauseTiming();
    for (unsigned i=0; i<19; i++) {
      for (unsigned j=0; j<19; j++) {
        if (std::abs(::D3Q19::e_ki[i][j]) > std::numeric_limits<double>::epsilon())
          m(i, j) = ::D3Q19::e_ki[i][j];
      }
    }
    blaze::StaticVector<double, 19UL> n;
    std::generate(n.begin(), n.end(), [](){return dist(mt);});
    blaze::StaticVector<double, 19UL> res;
    state.ResumeTiming();
    lb_calc_m_from_n_blaze(m, n, res);
  }
}

BENCHMARK(BM_LB_Modes_Calculation_blaze);

BENCHMARK_MAIN();
     
