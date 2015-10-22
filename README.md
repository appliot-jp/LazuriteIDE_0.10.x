# LazuriteIDE_0.10.x
the latest hardware, libraries, and examples source code

LazuriteIDEの中でML620Q504に関連するソフトウエアのソースコードを保存しているプロジェクトです。
本レポジトリをLazuriteIDEのフォルダに関連付けることで、常に最新のソースコードに保つことができます。

# WatchDogTimerについて
2015.10.22にWatch Dog Timerを使用することが出来るオプションを公開しました。

Lazurite BasicでWatch Dog Timerを使用するためには、オプションでLazurite Basic(WatchDogTimer)を指定してください。
Lazurite Sub-GHzでWatch Dog Timerを使用するためには、オプションでLazurite Sub-GHz(WatchDogTimer)を指定してください。

Watch Dog Timerをクリアするためには、wdt.hをインクルードして、wdt_clear(void)関数を実行する必要があります。
デフォルトのwatch dog timerは約8秒が設定されています。2回分(16秒間) wdt_clearが実行されないと、マイコンは自動的にリセットします。


