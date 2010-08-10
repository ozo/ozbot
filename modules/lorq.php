<?php
	$source = file_get_contents("http://www.lorquotes.ru/rss/main.xml");
	preg_match_all("/<description>(.*?)<\/description>/i", $source, $quote);
	echo iconv("Windows-1251", "UTF-8", "Свеженькое с lorquotes.ru \n").preg_replace("/<(.*?)>/i", "", preg_replace("/<br(.*?)>/i", "\n", html_entity_decode(iconv("KOI8-R", "UTF-8", $quote[1][1]))));
?>
