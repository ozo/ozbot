<?php
	$source = file_get_contents("http://ibash.org.ru/random.php");
	preg_match("/<div class=\"quotbody\">(.*?)<\/div>/i",$source,$quote);
	echo preg_replace("/<(.*?)>/i", "", preg_replace("/<br(.*?)>/i", "\n", html_entity_decode(iconv("Windows-1251", "UTF-8", $quote[1]))));
?>
