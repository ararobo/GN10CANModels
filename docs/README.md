# Documentation

このディレクトリには、gn10-can プロジェクトの設計ドキュメントが含まれています。

## ドキュメント一覧

### ブランチ比較

- **[feat-branches-comparison.md](feat-branches-comparison.md)** (日本語)
  - 3つの feat/* ブランチ（proposal-1, proposal-4, proposal-5）の違いと特徴を詳しく比較・分析したドキュメント
  
- **[feat-branches-comparison_en.md](feat-branches-comparison_en.md)** (English)
  - English version of the feat/* branches comparison document

### 設計提案

#### design/ ディレクトリ

各設計提案の詳細ドキュメント:

1. **[01_interface_injection.md](design/01_interface_injection.md)** - インターフェース分離による依存関係の逆転
2. **[02_callback_decoupling.md](design/02_callback_decoupling.md)** - コールバック関数による疎結合化
3. **[03_port_mediator.md](design/03_port_mediator.md)** - 中間「ポート」オブジェクトによる依存の分断
4. **[04_simple_forward_decl.md](design/04_simple_forward_decl.md)** - 前方宣言を活用したシンプル構成
5. **[05_modern_auto_register.md](design/05_modern_auto_register.md)** - RAIIによる自動登録とモダンAPI
6. **[comparison_summary.md](design/comparison_summary.md)** - 設計提案の比較まとめ

## クイックガイド

### feat/* ブランチを理解したい場合

→ **[feat-branches-comparison.md](feat-branches-comparison.md)** をご覧ください

このドキュメントには以下が含まれています:
- 3つのブランチの比較表
- 各ブランチの詳細な説明
- アーキテクチャ図
- 使用例のコード
- メリット・デメリット
- 選択基準と推奨事項

### 各提案の詳細を知りたい場合

→ **design/** フォルダ内の各提案ドキュメントをご覧ください

---

# Documentation (English)

This directory contains design documentation for the gn10-can project.

## Document List

### Branch Comparison

- **[feat-branches-comparison_en.md](feat-branches-comparison_en.md)** (English)
  - Detailed comparison and analysis of the three feat/* branches (proposal-1, proposal-4, proposal-5)
  
- **[feat-branches-comparison.md](feat-branches-comparison.md)** (日本語)
  - Japanese version of the feat/* branches comparison document

### Design Proposals

#### design/ Directory

Detailed documents for each design proposal:

1. **[01_interface_injection.md](design/01_interface_injection.md)** - Dependency Inversion through Interface Separation
2. **[02_callback_decoupling.md](design/02_callback_decoupling.md)** - Decoupling with Callback Functions
3. **[03_port_mediator.md](design/03_port_mediator.md)** - Dependency Separation with Intermediate "Port" Objects
4. **[04_simple_forward_decl.md](design/04_simple_forward_decl.md)** - Simple Structure Using Forward Declaration
5. **[05_modern_auto_register.md](design/05_modern_auto_register.md)** - Auto-Registration and Modern API with RAII
6. **[comparison_summary.md](design/comparison_summary.md)** - Design Proposal Comparison Summary

## Quick Guide

### To understand feat/* branches

→ See **[feat-branches-comparison_en.md](feat-branches-comparison_en.md)**

This document includes:
- Comparison table of three branches
- Detailed explanation of each branch
- Architecture diagrams
- Usage example code
- Benefits and drawbacks
- Selection criteria and recommendations

### For detailed information on each proposal

→ See individual proposal documents in the **design/** folder
