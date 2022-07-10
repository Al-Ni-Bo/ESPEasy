#ifndef HELPERS_PLUGINSTATS_H
#define HELPERS_PLUGINSTATS_H

#include "../../ESPEasy_common.h"

#ifdef USES_PLUGIN_STATS

# include "../DataStructs/ChartJS_dataset_config.h"

# include <CircularBuffer.h>

# ifndef PLUGIN_STATS_NR_ELEMENTS
#  ifdef ESP8266
#   define PLUGIN_STATS_NR_ELEMENTS 16
#  endif // ifdef ESP8266
#  ifdef ESP32
#   define PLUGIN_STATS_NR_ELEMENTS 64
#  endif // ifdef ESP32
# endif  // ifndef PLUGIN_STATS_NR_ELEMENTS

class PluginStats {
public:

  typedef CircularBuffer<float, PLUGIN_STATS_NR_ELEMENTS> PluginStatsBuffer_t;

  PluginStats() = delete;
  PluginStats(uint8_t nrDecimals,
              float   errorValue);


  // Add a sample to the _sample buffer
  // This does not also track peaks as the peaks could be raw sensor data and the samples processed data.
  bool push(float value);

  // Keep track of peaks.
  // Use this for sensors that need to take several samples before actually output a task value.
  // For example the ADC with oversampling
  void  trackPeak(float value);

  // Get lowest recorded value since reset
  float getPeakLow() const {
    return hasPeaks() ? _minValue : _errorValue;
  }

  // Get highest recorded value since reset
  float getPeakHigh() const {
    return hasPeaks() ? _maxValue : _errorValue;
  }

  bool hasPeaks() const {
    return _maxValue >= _minValue;
  }

  // Set the peaks to unset values
  void resetPeaks();

  void clearSamples() {
    _samples.clear();
  }

  size_t getNrSamples() const {
    return _samples.size();
  }

  // Compute average over all stored values
  float getSampleAvg() const {
    return getSampleAvg(_samples.size());
  }

  // Compute average over last N stored values
  float getSampleAvg(PluginStatsBuffer_t::index_t lastNrSamples) const;

  float operator[](PluginStatsBuffer_t::index_t index) const;


  // Support task value notation to 'get' statistics
  // Notations like [taskname#taskvalue.avg] can then be used to compute the average over a number of samples.
  bool plugin_get_config_value_base(struct EventStruct *event,
                                    String            & string) const;

  bool webformLoad_show_stats(struct EventStruct *event) const;

# ifdef USES_CHART_JS
  void plot_ChartJS_dataset() const;
# endif // ifdef USES_CHART_JS

#ifdef USES_CHART_JS
  ChartJS_dataset_config _ChartJS_dataset_config;
#else
  String _label;
#endif

private:

  float _minValue;
  float _maxValue;

  PluginStatsBuffer_t _samples;
  float _errorValue;

  uint8_t _nrDecimals = 3u;
};
#endif // ifdef USES_PLUGIN_STATS
#endif // ifndef HELPERS_PLUGINSTATS_H
