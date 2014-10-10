/**
 * @file naive_kmeans_impl.hpp
 * @author Ryan Curtin
 *
 * An implementation of a naively-implemented step of the Lloyd algorithm for
 * k-means clustering.  This may still be the best choice for small datasets or
 * datasets with very high dimensionality.
 */
#ifndef __MLPACK_METHODS_KMEANS_NAIVE_KMEANS_IMPL_HPP
#define __MLPACK_METHODS_KMEANS_NAIVE_KMEANS_IMPL_HPP

// In case it hasn't been included yet.
#include "naive_kmeans.hpp"

namespace mlpack {
namespace kmeans {

template<typename MetricType, typename MatType>
NaiveKMeans<MetricType, MatType>::NaiveKMeans(const MatType& dataset,
                                              MetricType& metric) :
    dataset(dataset),
    metric(metric),
    distanceCalculations(0)
{ /* Nothing to do. */ }

// Run a single iteration.
template<typename MetricType, typename MatType>
double NaiveKMeans<MetricType, MatType>::Iterate(const arma::mat& centroids,
                                                 arma::mat& newCentroids,
                                                 arma::Col<size_t>& counts)
{
  newCentroids.zeros(centroids.n_rows, centroids.n_cols);
  counts.zeros(centroids.n_cols);

  // Find the closest centroid to each point and update the new centroids.
  for (size_t i = 0; i < dataset.n_cols; i++)
  {
    // Find the closest centroid to this point.
    double minDistance = std::numeric_limits<double>::infinity();
    size_t closestCluster = centroids.n_cols; // Invalid value.

    for (size_t j = 0; j < centroids.n_cols; j++)
    {
      const double distance = metric.Evaluate(dataset.col(i), centroids.col(j));

      if (distance < minDistance)
      {
        minDistance = distance;
        closestCluster = j;
      }
    }

    Log::Assert(closestCluster != centroids.n_cols);

    // We now have the minimum distance centroid index.  Update that centroid.
    newCentroids.col(closestCluster) += arma::vec(dataset.col(i));
    counts(closestCluster)++;
  }

  // Now normalize the centroid.
  for (size_t i = 0; i < centroids.n_cols; ++i)
    if (counts(i) != 0)
      newCentroids.col(i) /= counts(i);
    else
      newCentroids.col(i).fill(DBL_MAX); // Invalid value.

  distanceCalculations += centroids.n_cols * dataset.n_cols;

  // Calculate cluster distortion for this iteration.
  double cNorm = 0.0;
  for (size_t i = 0; i < centroids.n_cols; ++i)
  {
    const double dist = std::pow(
        metric.Evaluate(centroids.col(i), newCentroids.col(i)), 2.0);
    cNorm += std::pow(dist, 2.0);
  }
  return std::sqrt(cNorm);
}

} // namespace kmeans
} // namespace mlpack

#endif
