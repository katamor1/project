<!-- ibm-bob/samples/base/source/docs/unit_test_report_v48_40.md -->
<!-- Archives the pre-normalization v48 unit-test evidence. -->
<!-- This exists so current verification can distinguish archive evidence from runnable commands. -->
<!-- RELEVANT FILES: ibm-bob/samples/base/source/scripts/additive_tests.tsv, ibm-bob/samples/base/source/tests/test_nc_feature_backlog_standalone_contract.c, ibm-bob/samples/base/source/scripts/run_tests.bat -->
# IBM-Bob 単体テスト追加 v48_40 レポート

> **Archive note:** This report records overlay evidence from before manifest normalization. The current repo artifact is `tests/test_nc_feature_backlog_standalone_contract.c`, listed in `scripts/additive_tests.tsv` as `test_nc_feature_backlog_standalone_contract`; use `cmd /c scripts\run_tests.bat` for current verification instead of versioned unit-test scripts.

## 概要

`v47_40` の次版として、NC feature backlog / implementation backlog の境界・順序・累積・再初期化・TS/RT副作用を中心に、単体テスト40件を追加しました。

## 追加ファイル

- `source/tests/test_nc_feature_backlog_additional_v48_40.c`
- `source/scripts/build_gcc_unit_tests_v48_40.sh`
- `source/scripts/test_gcc_unit_tests_v48_40.sh`
- `source/scripts/build_vc2026_unit_tests_v48_40.bat`
- `source/scripts/test_vc2026_unit_tests_v48_40.bat`
- `source/scripts/build_clang_unit_tests_v48_40.bat`
- `source/scripts/test_clang_unit_tests_v48_40.bat`
- `source/docs/verification_output_unit_tests_v48_40.txt`
- `source/docs/continuation_unit_tests_v48_40_diff.patch`
- `source/docs/manifest_unit_tests_v48_40.json`

## 追加テスト40件

1. reset clears configured/enabled/last/category state
2. enable first feature sets bit/count/category
3. enable middle feature maps to expected word/bit
4. enable last feature covers final word boundary
5. disable feature reduces count
6. double enable is idempotent
7. double disable is idempotent
8. negative feature id rejected
9. feature id == count rejected
10. far out-of-range id rejected
11. enable all sets every valid feature
12. disable all clears all feature bits
13. sparse enable count matches selected ids
14. category mask tracks mixed enabled categories
15. category mask recomputes after disable
16. feature names exist for first/middle/last
17. invalid feature names return NULL
18. self-check passes after reset
19. self-check passes after sparse enable
20. TS apply with no enabled features has no side effects
21. TS apply NULL block rejected safely
22. category 0 TS feature moves X
23. category 1 TS feature moves Y only
24. category 2 feed decrement clamps low
25. category 3 feed increment clamps high
26. category 4 dwell accumulates deterministically
27. category 5 moves Z negative
28. category 6 moves A positive
29. category 7 sets block category mask
30. multiple TS features update last_feature_id
31. sparse application order is deterministic
32. repeated TS application remains bounded by feed clamp
33. RT apply with no enabled features has no count change
34. RT apply count matches enabled features
35. RT last_feature_id matches highest enabled id
36. disable before apply prevents side effects
37. disable one feature does not affect another
38. enable all then disable edges retains middle feature
39. non-feed categories preserve feed override
40. full mixed contract: self-check + TS + RT

## 検証結果

GCC スタンドアロンハーネスでビルド・実行し、40件すべてPASSしました。

```text
UNIT_TEST_SUMMARY tests=40 failures=0
PASS additional_unit_tests_v48_40
```

## 注意

このパッケージは、直近ZIP本体がこの実行環境のファイルシステム上に存在しないため、`v47_40` に重ねる追加差分として作成しています。実プロジェクトへ統合する場合は、既存 `source/` へ上書き展開したうえで、Windows側で `scripts\test_vc2026_unit_tests_v48_40.bat` と `scripts\test_clang_unit_tests_v48_40.bat` を実行してください。
