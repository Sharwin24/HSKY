/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package itzfx.fxml.tabs;

import itzfx.Start;
import java.time.Instant;
import java.time.LocalDateTime;
import java.time.ZoneId;
import java.time.format.DateTimeFormatter;
import java.util.concurrent.ForkJoinPool;
import java.util.concurrent.TimeUnit;
import javafx.application.Platform;
import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.fxml.FXML;
import javafx.scene.chart.AreaChart;
import javafx.scene.chart.NumberAxis;
import javafx.scene.chart.XYChart;
import javafx.util.StringConverter;

/**
 * FXML Controller class
 *
 * @author prem
 */
public class SystemStatsController {

    private final com.sun.management.OperatingSystemMXBean osConnection = (com.sun.management.OperatingSystemMXBean) java.lang.management.ManagementFactory.getOperatingSystemMXBean();
    private final java.lang.management.MemoryMXBean memoryConnection = java.lang.management.ManagementFactory.getMemoryMXBean();
    @FXML
    private AreaChart<Number, Number> memoryChart;
    @FXML
    private AreaChart<Number, Number> processCPUChart;
    @FXML
    private AreaChart<Number, Number> systemCPUChart;

    private static final int MILLION = 1000000;
    
    @FXML
    private void initialize() {
        final ObservableList<XYChart.Data<Number, Number>> memorySeries = FXCollections.observableArrayList();
        final ObservableList<XYChart.Data<Number, Number>> usedMemorySeries = FXCollections.observableArrayList();
        final ObservableList<XYChart.Data<Number, Number>> processCPUSeries = FXCollections.observableArrayList();
        final ObservableList<XYChart.Data<Number, Number>> systemCPUSeries = FXCollections.observableArrayList();
        memoryChart.getData().addAll(new XYChart.Series<>(memorySeries), new XYChart.Series<>(usedMemorySeries));
        processCPUChart.getData().add(new XYChart.Series<>(processCPUSeries));
        systemCPUChart.getData().add(new XYChart.Series<>(systemCPUSeries));
        ((NumberAxis) memoryChart.getXAxis()).setTickLabelFormatter(new DateFormatter());
        ((NumberAxis) processCPUChart.getXAxis()).setTickLabelFormatter(new DateFormatter());
        ((NumberAxis) systemCPUChart.getXAxis()).setTickLabelFormatter(new DateFormatter());
        ((NumberAxis) processCPUChart.getYAxis()).setTickLabelFormatter(new PercentFormatter());
        ((NumberAxis) systemCPUChart.getYAxis()).setTickLabelFormatter(new PercentFormatter());
        Start.PULSER.scheduleAtFixedRate(() -> ForkJoinPool.commonPool().execute(() -> {
            long time = System.currentTimeMillis();
            java.lang.management.MemoryUsage usage = memoryConnection.getHeapMemoryUsage();
            long committedMemory = usage.getCommitted();
            long usedMemory = usage.getUsed();
            if (usedMemory > 500 * MILLION) {
                memoryConnection.gc();
            }
            double processLoad = osConnection.getProcessCpuLoad();
            double systemLoad = osConnection.getSystemCpuLoad();
            Platform.runLater(() -> {
                memorySeries.add(new XYChart.Data<>(time, committedMemory / MILLION));
                usedMemorySeries.add(new XYChart.Data<>(time, usedMemory / MILLION));
                processCPUSeries.add(new XYChart.Data<>(time, processLoad * 100));
                systemCPUSeries.add(new XYChart.Data<>(time, systemLoad * 100));
            });
        }), 0, 1, TimeUnit.SECONDS);
    }

    private static final class DateFormatter extends StringConverter<Number> {

        @Override
        public String toString(Number n) {
            return formatDate(n.longValue());
        }

        @Override
        public Number fromString(String s) {
            return 0;
        }
    }

    private static String formatDate(long millis) {
        return LocalDateTime.ofInstant(Instant.ofEpochMilli(millis), ZoneId.systemDefault()).format(DateTimeFormatter.ofPattern("hh:mm:ss.SSS a"));
    }
    
    private static final class PercentFormatter extends StringConverter<Number> {
        @Override
        public String toString(Number n) {
            return n.doubleValue() + "%";
        }
        
        @Override
        public Number fromString(String s) {
            return Double.parseDouble(s.substring(0, s.length() - 1));
        }
    }
}
