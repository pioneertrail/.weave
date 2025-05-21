#include "chronovyan/ml_model.hpp"
#include <algorithm>
#include <numeric>
#include <cmath>
#include <random>
#include <stdexcept>
#include <optional>

namespace chronovyan {
namespace sync {

MLModel::MLModel(
    const std::string& model_type,
    const std::vector<std::string>& feature_columns,
    double learning_rate,
    std::optional<unsigned int> seed
) : model_type(model_type),
    feature_columns(feature_columns),
    learning_rate(learning_rate),
    seed(seed) {
    initialize_model();
}

void MLModel::initialize_model() {
    // Initialize feature weights with small random values
    std::mt19937 gen;
    if (seed) {
        gen.seed(*seed);
    } else {
        std::random_device rd;
        gen.seed(rd());
    }
    std::normal_distribution<double> dist(0.0, 0.01);
    
    feature_weights.resize(feature_columns.size());
    for (auto& weight : feature_weights) {
        weight = dist(gen);
    }
    
    feature_importance.resize(feature_columns.size(), 0.0);
    
    // Initialize hyperparameters
    hyperparameters["learning_rate"] = learning_rate;
    hyperparameters["regularization"] = 0.01;
    hyperparameters["momentum"] = 0.9;
    hyperparameters["max_depth"] = 3;
    hyperparameters["min_samples_split"] = 2;
}

void MLModel::update(const std::vector<double>& features) {
    if (features.size() != feature_columns.size()) {
        throw std::invalid_argument("Feature size mismatch");
    }
    
    // Check for NaN or Inf values
    for (double feature : features) {
        if (std::isnan(feature) || std::isinf(feature)) {
            throw std::invalid_argument("Invalid feature value (NaN or Inf)");
        }
    }
    
    train_model(features);
    update_feature_importance();
}

void MLModel::update_hyperparameters(const OptimizationMetrics& metrics) {
    // Update hyperparameters based on optimization metrics
    double efficiency_factor = metrics.sync_efficiency;
    double stability_factor = metrics.stability;
    double coherence_factor = 1.0 - metrics.error_rate / 100.0; // Convert error rate to coherence score
    
    // Adjust learning rate based on efficiency
    hyperparameters["learning_rate"] *= (1.0 + efficiency_factor * 0.1);
    
    // Adjust regularization based on stability
    hyperparameters["regularization"] *= (1.0 - stability_factor * 0.1);
    
    // Adjust momentum based on coherence
    hyperparameters["momentum"] *= (1.0 + coherence_factor * 0.1);
    
    // Ensure hyperparameters stay within valid ranges
    hyperparameters["learning_rate"] = std::clamp(
        hyperparameters["learning_rate"],
        0.001,
        0.1
    );
    hyperparameters["regularization"] = std::clamp(
        hyperparameters["regularization"],
        0.001,
        0.1
    );
    hyperparameters["momentum"] = std::clamp(
        hyperparameters["momentum"],
        0.5,
        0.99
    );
}

double MLModel::predict(const std::vector<double>& features) const {
    if (features.size() != feature_columns.size()) {
        throw std::invalid_argument("Feature size mismatch");
    }
    
    // Check for NaN or Inf values
    for (double feature : features) {
        if (std::isnan(feature) || std::isinf(feature)) {
            throw std::invalid_argument("Invalid feature value (NaN or Inf)");
        }
    }
    
    // Gradient boosting prediction
    double prediction = 0.0;
    for (size_t i = 0; i < features.size(); ++i) {
        prediction += features[i] * feature_weights[i];
    }
    
    // Apply sigmoid activation
    return 1.0 / (1.0 + std::exp(-prediction));
}

std::vector<double> MLModel::get_feature_importance() const {
    return feature_importance;
}

void MLModel::train_model(const std::vector<double>& features) {
    // Gradient boosting training
    double prediction = predict(features);
    double gradient = prediction * (1.0 - prediction);
    
    // Update weights using gradient descent with momentum
    for (size_t i = 0; i < features.size(); ++i) {
        double weight_update = learning_rate * gradient * features[i];
        feature_weights[i] -= weight_update;
        
        // Apply regularization
        feature_weights[i] *= (1.0 - hyperparameters["regularization"]);
        
        // Apply momentum
        feature_weights[i] *= hyperparameters["momentum"];
    }
}

void MLModel::update_feature_importance() {
    // Calculate feature importance based on weight magnitudes and feature values
    double total_weight = 0.0;
    std::vector<double> weight_magnitudes(feature_weights.size());
    
    // Calculate weight magnitudes
    for (size_t i = 0; i < feature_weights.size(); ++i) {
        weight_magnitudes[i] = std::abs(feature_weights[i]);
        total_weight += weight_magnitudes[i];
    }
    
    if (total_weight > 0.0) {
        // Normalize feature importance
        for (size_t i = 0; i < feature_weights.size(); ++i) {
            // Weight magnitude contributes 70% to importance
            double weight_importance = weight_magnitudes[i] / total_weight;
            
            // Feature value contributes 30% to importance
            double feature_value_importance = std::abs(feature_weights[i]) / 
                (std::abs(feature_weights[i]) + 1.0); // Add 1.0 to prevent division by zero
            
            feature_importance[i] = 0.7 * weight_importance + 0.3 * feature_value_importance;
        }
    }
}

} // namespace sync
} // namespace chronovyan 