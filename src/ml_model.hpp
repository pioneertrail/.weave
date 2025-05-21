#pragma once

#include <string>
#include <vector>
#include <map>
#include <optional>
#include "chronovyan/optimization_metrics.hpp"

namespace chronovyan {
namespace sync {

class MLModel {
public:
    MLModel(
        const std::string& model_type,
        const std::vector<std::string>& feature_columns,
        double learning_rate = 0.01,
        std::optional<unsigned int> seed = std::nullopt
    );

    void update(const std::vector<double>& features);
    void update_hyperparameters(const OptimizationMetrics& metrics);
    double predict(const std::vector<double>& features) const;
    std::vector<double> get_feature_importance() const;

private:
    void initialize_model();
    void train_model(const std::vector<double>& features);
    void update_feature_importance();

    std::string model_type;
    std::vector<std::string> feature_columns;
    std::vector<double> feature_weights;
    std::vector<double> feature_importance;
    std::map<std::string, double> hyperparameters;
    double learning_rate;
    std::optional<unsigned int> seed;
};

} // namespace sync
} // namespace chronovyan 