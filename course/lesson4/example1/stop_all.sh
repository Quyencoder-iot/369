#!/bin/bash
# ============================================
# STOP ALL - Dừng tất cả processes
# ============================================

echo "Stopping all Dashboard processes..."

pkill -f dashboard_service 2>/dev/null || true
pkill -f dashboard_client 2>/dev/null || true
pkill -f qt_dashboard 2>/dev/null || true
pkill -f qt_simple_client 2>/dev/null || true

echo "[OK] All processes stopped"
