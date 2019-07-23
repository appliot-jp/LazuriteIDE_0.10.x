# LazuriteIDE_0.10.x
the latest hardware, libraries, and examples source code

LazuriteIDEの中でML620Q504に関連するソフトウエアのソースコードを保存しているプロジェクトです。
本レポジトリをLazuriteIDEのフォルダに関連付けることで、常に最新のソースコードに保つことができます。

# WatchDogTimerについて
2015.10.22にWatch Dog Timerを使用することが出来るオプションを公開しました。

Lazurite BasicでWatch Dog Timerを使用するためには、オプションでLazurite Basic(WatchDogTimer)を指定してください。
Lazurite Sub-GHzでWatch Dog Timerを使用するためには、オプションでLazurite Sub-GHz(WatchDogTimer)を指定してください。
Lazurite mini seriesでWatch Dog Timerを使用するためには、オプションでLazurite mini series(WatchDogTimer)を指定してください。

Watch Dog Timerをクリアするためには、wdt.hをインクルードして、wdt_clear(void)関数を実行する必要があります。
デフォルトのwatch dog timerは約8秒が設定されています。2回分(16秒間) wdt_clearが実行されないと、マイコンは自動的にリセットします。
従来どおり、Lazurite Basic, Lazurite Sub-GHzを選択すればwatch dog timerは無効になっています。

Watch Dog Timerとはシステムを監視するためのタイマーです。一定時間以上クリアが行われないとプログラムが暴走したと判断し、自動的にマイコンの動作がリセットされる機能です。
