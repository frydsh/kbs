<?php
require("wapfuncs.php");
if(loginok())
{
  $brdstr = $_GET[n];
  $argv = decarg("searchboard",(key ($_GET)));
  $page = intval($argv["page"]);
  waphead(0);
  echo "<card id=\"main\" title=\"".BBS_WAP_NAME."\">";  
  echo "<p/>";
  echo "δ���<br/>";
  showlink(urlstr("menu"),"���˵�");
}
?>
</card>
</wml>
