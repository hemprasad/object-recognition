#include "Classify.hpp"
#include "feature.hpp"
#include <cmath>

const std::vector<Label> Classifier::labels = {Label::Nothing, Label::Ball};

using namespace std;

Classifier::Classifier()
{

}

Classifier::~Classifier()
{
}

void Classifier::clear_probs()
{
  for (int x = 0; x < Features::width; ++x) {
    for (int y = 0; y < Features::height; ++y) {
      for (int bin = 0; bin < Features::bins; ++bin) {
        feat_count[Label::Nothing][tuple<int, int, int>(x, y, bin)] = 0;
        feat_count[Label::Ball][tuple<int, int, int>(x, y, bin)] = 0;
        labels_count[Label::Ball] = 0;
        labels_count[Label::Nothing] = 0;
      }
    }
  }
}

void Classifier::train_datum(const Features& datum, Label label)
{
  labels_count[label] += 1;
  for (int x = 0; x < Features::width; ++x) {
    for (int y = 0; y < Features::height; ++y) {
      int bin = datum.pixels.at<unsigned char> (y, x);
      feat_count[label][tuple<int, int, int>(x, y, bin)] += 1;
    }
  }
}

void Classifier::calculate_probs(int smoothing)
{
  for (int x = 0; x < Features::width; ++x) {
    for (int y = 0; y < Features::height; ++y) {
      for (int bin = 0; bin < Features::bins; ++bin) {
        for (Label label : Classifier::labels) {
          tuple<int, int, int> feat(x, y, bin);
          int prob_there = (feat_count[label][feat] + smoothing)/
            (labels_count[label] + smoothing);
          feat_prob[Label::Nothing][feat] = prob_there;
        }
      }
    }
  }
}

Label Classifier::classify(const Features& datum)
{
  double prior = 1;
  map<Label, double> probs;
  for (Label label : Classifier::labels) {
    probs[label] = prior;
    for (int x = 0; x < Features::width; ++x) {
      for (int y = 0; y < Features::height; ++y) {
        int bin = datum.pixels.at<unsigned char>(y, x);
        probs[label] += log( feat_prob[label][tuple<int, int, int>(x, y, bin)] );
      }
    }
  }
  
  auto max_iter = max_element(probs.begin(), probs.end(),
    [](const pair<Label, double>& p1, const pair<Label, double>& p2) {
      return p1.second < p2.second;
    });

  return max_iter->first;
}
