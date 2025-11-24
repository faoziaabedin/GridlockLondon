// code/analytics/ReportExporter.cpp
#include "ReportExporter.h"
#include "../core/SimulationController.h"
#include "../core/Metrics.h"
#include "../core/City.h"
#include <QFile>
#include <QTextStream>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDateTime>

ReportExporter::ReportExporter() {
}

bool ReportExporter::exportPDF(const QString& filepath,
                               const SimulationController& controller,
                               const TrafficFlowAnalyzer& flowAnalyzer,
                               const PolicyEffectivenessAnalyzer& policyAnalyzer,
                               const PredictiveAnalyzer& predictiveAnalyzer) {
    // Note: Full PDF generation would require a library like QPrinter or external tool
    // For now, export as HTML that can be converted to PDF
    
    QString html = R"(<!DOCTYPE html>
<html>
<head>
    <title>GridlockLondon Analytics Report</title>
    <style>
        body { font-family: Arial, sans-serif; margin: 20px; }
        h1 { color: #007ACC; }
        h2 { color: #3B82F6; margin-top: 30px; }
        table { border-collapse: collapse; width: 100%; margin: 20px 0; }
        th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }
        th { background-color: #252526; color: white; }
        .metric { background-color: #f5f5f5; padding: 10px; margin: 10px 0; }
    </style>
</head>
<body>
)";
    
    html += "<h1>GridlockLondon Traffic Simulation Report</h1>";
    html += "<p><strong>Generated:</strong> " + QDateTime::currentDateTime().toString() + "</p>";
    
    // Add report content
    html += generateReportContent(controller, flowAnalyzer, policyAnalyzer, predictiveAnalyzer);
    
    html += R"(
</body>
</html>
)";
    
    // Save as HTML (can be converted to PDF by browser)
    QFile file(filepath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }
    
    QTextStream out(&file);
    out << html;
    file.close();
    
    return true;
}

bool ReportExporter::exportPowerPoint(const QString& filepath,
                                      const SimulationController& controller,
                                      const TrafficFlowAnalyzer& flowAnalyzer) {
    // Note: PowerPoint export would require external library
    // For now, export as structured text that can be imported
    
    QFile file(filepath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }
    
    QTextStream out(&file);
    out << "GridlockLondon Analytics Presentation\n";
    out << "=====================================\n\n";
    
    out << "Slide 1: Overview\n";
    out << "-----------------\n";
    // Add overview data
    
    out << "\nSlide 2: Traffic Flow Analysis\n";
    out << "------------------------------\n";
    auto hotspots = flowAnalyzer.detectHotspots(*controller.getCity());
    for (const auto& hotspot : hotspots) {
        out << QString("Edge %1: Utilization %2%\n")
               .arg(hotspot.edgeId)
               .arg(hotspot.utilization * 100.0, 0, 'f', 1);
    }
    
    file.close();
    return true;
}

bool ReportExporter::exportCSV(const QString& filepath,
                               const SimulationController& controller,
                               const TrafficFlowAnalyzer& flowAnalyzer,
                               bool includeMetadata) {
    QFile file(filepath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }
    
    QTextStream out(&file);
    
    // Metadata header
    if (includeMetadata) {
        out << "# GridlockLondon Analytics Export\n";
        out << "# Generated: " << QDateTime::currentDateTime().toString() << "\n";
        out << "# Format: CSV with metadata\n";
        out << "#\n";
    }
    
    // CSV header
    out << "EdgeId,Occupancy,Capacity,Utilization,CongestionLevel\n";
    
    // Data rows
    auto heatmap = flowAnalyzer.getUtilizationHeatmap(*controller.getCity());
    for (const auto& pair : heatmap) {
        const auto& data = pair.second;
        out << QString("%1,%2,%3,%4,%5\n")
               .arg(data.edgeId)
               .arg(data.occupancy)
               .arg(data.capacity)
               .arg(data.utilization, 0, 'f', 3)
               .arg(flowAnalyzer.calculateUtilization(*controller.getCity(), data.edgeId), 0, 'f', 3);
    }
    
    file.close();
    return true;
}

QString ReportExporter::exportJSON(const SimulationController& controller,
                                  const TrafficFlowAnalyzer& flowAnalyzer,
                                  const PolicyEffectivenessAnalyzer& policyAnalyzer,
                                  const PredictiveAnalyzer& predictiveAnalyzer) {
    QJsonObject root;
    
    // Metadata
    root["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    root["version"] = "1.0";
    root["application"] = "GridlockLondon";
    
    // Metrics
    if (controller.getMetrics()) {
        QJsonObject metrics;
        metrics["averageTripTime"] = controller.getMetrics()->averageTripTime();
        metrics["totalThroughput"] = controller.getMetrics()->totalThroughput();
        metrics["maxEdgeLoad"] = controller.getMetrics()->maxEdgeLoad();
        root["metrics"] = metrics;
    }
    
    // Hotspots
    if (controller.getCity()) {
        auto hotspots = flowAnalyzer.detectHotspots(*controller.getCity());
        QJsonArray hotspotArray;
        for (const auto& hotspot : hotspots) {
            QJsonObject h;
            h["edgeId"] = static_cast<int>(hotspot.edgeId);
            h["congestionLevel"] = hotspot.congestionLevel;
            h["occupancy"] = hotspot.currentOccupancy;
            h["capacity"] = hotspot.capacity;
            h["utilization"] = hotspot.utilization;
            hotspotArray.append(h);
        }
        root["hotspots"] = hotspotArray;
    }
    
    // Flow data
    if (controller.getCity()) {
        auto flowData = flowAnalyzer.getFlowData(controller.getAgents());
        QJsonArray flowArray;
        for (const auto& flow : flowData) {
            QJsonObject f;
            f["from"] = static_cast<int>(flow.from);
            f["to"] = static_cast<int>(flow.to);
            f["agentCount"] = flow.agentCount;
            f["averageTime"] = flow.averageTime;
            flowArray.append(f);
        }
        root["flowData"] = flowArray;
    }
    
    QJsonDocument doc(root);
    return doc.toJson(QJsonDocument::Indented);
}

bool ReportExporter::saveJSON(const QString& filepath, const QString& jsonData) {
    QFile file(filepath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }
    
    QTextStream out(&file);
    out << jsonData;
    file.close();
    
    return true;
}

QString ReportExporter::generateReportContent(
    const SimulationController& controller,
    const TrafficFlowAnalyzer& flowAnalyzer,
    const PolicyEffectivenessAnalyzer& policyAnalyzer,
    const PredictiveAnalyzer& predictiveAnalyzer) {
    
    QString content;
    
    // Metrics section
    if (controller.getMetrics()) {
        content += "<h2>Simulation Metrics</h2>\n";
        content += formatMetrics(*controller.getMetrics());
    }
    
    // Hotspots section
    if (controller.getCity()) {
        content += "<h2>Traffic Hotspots</h2>\n";
        auto hotspots = flowAnalyzer.getTopBottlenecks(*controller.getCity(), 10);
        
        content += "<table>\n";
        content += "<tr><th>Edge ID</th><th>Utilization</th><th>Congestion Level</th></tr>\n";
        
        for (const auto& hotspot : hotspots) {
            content += QString("<tr><td>%1</td><td>%2%</td><td>%3</td></tr>\n")
                          .arg(hotspot.edgeId)
                          .arg(hotspot.utilization * 100.0, 0, 'f', 1)
                          .arg(hotspot.congestionLevel, 0, 'f', 2);
        }
        content += "</table>\n";
    }
    
    return content;
}

QString ReportExporter::formatMetrics(const Metrics& metrics) {
    QString html = "<div class=\"metric\">\n";
    html += QString("<p><strong>Average Trip Time:</strong> %1 steps</p>\n")
               .arg(metrics.averageTripTime(), 0, 'f', 2);
    html += QString("<p><strong>Total Throughput:</strong> %1 agents</p>\n")
               .arg(metrics.totalThroughput());
    html += QString("<p><strong>Max Edge Load:</strong> %1 vehicles</p>\n")
               .arg(metrics.maxEdgeLoad());
    html += "</div>\n";
    return html;
}

