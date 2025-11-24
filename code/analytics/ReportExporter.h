// code/analytics/ReportExporter.h
#pragma once

#include <QString>
#include <QStringList>
#include <memory>
#include "../core/Metrics.h"
#include "TrafficFlowAnalyzer.h"
#include "PolicyEffectivenessAnalyzer.h"
#include "PredictiveAnalyzer.h"

class City;
class SimulationController;

/**
 * ReportExporter: Export analytics data in various formats
 * 
 * Supports:
 * - PDF reports (full analysis)
 * - PowerPoint export (charts as slides)
 * - CSV with rich metadata
 * - JSON API for external tools
 */
class ReportExporter {
public:
    ReportExporter();
    ~ReportExporter() = default;
    
    /**
     * Export comprehensive PDF report
     */
    bool exportPDF(const QString& filepath, 
                   const SimulationController& controller,
                   const TrafficFlowAnalyzer& flowAnalyzer,
                   const PolicyEffectivenessAnalyzer& policyAnalyzer,
                   const PredictiveAnalyzer& predictiveAnalyzer);
    
    /**
     * Export PowerPoint presentation
     */
    bool exportPowerPoint(const QString& filepath,
                         const SimulationController& controller,
                         const TrafficFlowAnalyzer& flowAnalyzer);
    
    /**
     * Export CSV with metadata
     */
    bool exportCSV(const QString& filepath,
                   const SimulationController& controller,
                   const TrafficFlowAnalyzer& flowAnalyzer,
                   bool includeMetadata = true);
    
    /**
     * Export JSON API format
     */
    QString exportJSON(const SimulationController& controller,
                      const TrafficFlowAnalyzer& flowAnalyzer,
                      const PolicyEffectivenessAnalyzer& policyAnalyzer,
                      const PredictiveAnalyzer& predictiveAnalyzer);
    
    /**
     * Save JSON to file
     */
    bool saveJSON(const QString& filepath, const QString& jsonData);
    
private:
    /**
     * Generate report content
     */
    QString generateReportContent(const SimulationController& controller,
                                  const TrafficFlowAnalyzer& flowAnalyzer,
                                  const PolicyEffectivenessAnalyzer& policyAnalyzer,
                                  const PredictiveAnalyzer& predictiveAnalyzer);
    
    /**
     * Format metrics for export
     */
    QString formatMetrics(const Metrics& metrics);
};

