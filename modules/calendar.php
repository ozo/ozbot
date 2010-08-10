<?php
	$source = file_get_contents("http://www.calend.ru/img/export/calend.rss");
	if ($source==FALSE)
	{
		echo iconv("Windows-1251", "UTF-8", "Что-то не так :(");
	}
	else
	{
		preg_match_all("/<item>(.*?)<\/item>/i", $source, $quote);
		for ($i=0; $i< count($quote[1]); $i++) {
			preg_match("/<title>(.*?)<\/title>/i", $quote[1][$i], $title);
			preg_match("/<category>(.*?)<\/category>/i", $quote[1][$i], $cat);
			//preg_match("/<!\[CDATA\[(.*?)\]\]/i", $quote[1][$i], $desc);
			echo iconv("Windows-1251", "UTF-8", $title[1]." (".$cat[1].")\n")/*.iconv("Windows-1251", "UTF-8", $desc[1])*/;
		}
	}
?>