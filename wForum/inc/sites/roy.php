<?php
require("default.php");

function getattachtmppath($userid,$utmpnum)
{
  $attachdir="cache/home/" . strtoupper(substr($userid,0,1)) . "/" . $userid . "/" . $utmpnum . "/upload"; 
  return $attachdir;
}

$SiteName="���˴�BBS";

$HTMLTitle="���˴�BBS";

$HTMLCharset="GB2312";

$DEFAULTStyle="style1.css";

$Banner="pic/ws.jpg";

$BannerURL="http://172.16.50.79";

//$SiteURL=$_SERVER['SERVER_NAME'];

$SiteURL="172.16.50.79";



define("ATTACHMAXSIZE","2097152");
define("ATTACHMAXCOUNT","3");
$section_nums = array("0", "1", "2", "3", "4");
$section_names = array(
    array("BBS ϵͳ", "[ϵͳ]"),
    array("У԰��Ϣ", "[��Ϣ]"),
    array("���Լ���", "[����]"),
    array("ѧ����ѧ", "[ѧϰ]"),
    array("��������", "[����]")
);
?>
