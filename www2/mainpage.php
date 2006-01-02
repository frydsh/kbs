<?php
require("site.php");
require("www2-board.php");

if (!bbs_ext_initialized())
	bbs_init_ext();

# get an attribute from a particular node
function find_attr($parent,$name,$attr)
{
    $nodes = $parent->child_nodes();
    while($node = array_shift($nodes))
        if ($node->node_name() == $name)
            return $node->get_attribute($attr);
    return "";
}

?>
<?php
function gen_hot_subjects_html()
{
# load xml doc
$hotsubject_file = BBS_HOME . "/xml/day.xml";
$doc = domxml_open_file($hotsubject_file);
	if (!$doc)
		return;

$root = $doc->document_element();
$boards = $root->child_nodes();


$brdarr = array();
?>
	<table width="97%" border="0" cellpadding="0" cellspacing="0" class="hot_title">
        <tr> 
		  <td width="23">&nbsp;</td>
          <td>&gt;&gt;�����ȵ㻰������&gt;&gt;</td>
        </tr>
	</table>
	<table border="0" cellpadding="0" cellspacing="0" width="97%">
              <tr> 
                <td class="MainContentText">
<ul style="margin: 5px 0px 20px 20px; padding: 0px;">
<?php
# shift through the array
while($board = array_shift($boards))
{
    if ($board->node_type() == XML_TEXT_NODE)
        continue;

    $hot_title = find_content($board, "title");
    $hot_author = find_content($board, "author");
    $hot_board = find_content($board, "board");
    $hot_time = find_content($board, "time");
    $hot_number = find_content($board, "number");
    $hot_groupid = find_content($board, "groupid");

	$brdnum = bbs_getboard($hot_board, $brdarr);
	if ($brdnum == 0)
		continue;
	$brd_encode = urlencode($brdarr["NAME"]);
?>
<li class="default">
<a href="bbstcon.php?board=<?php echo $brd_encode; ?>&gid=<?php echo $hot_groupid; ?>"><?php echo htmlspecialchars($hot_title); ?> </a>&nbsp;&nbsp;[����: <a href="bbsqry.php?userid=<?php echo $hot_author; ?>"><?php  echo $hot_author; ?></a>]&nbsp;&nbsp;&lt;<a href="bbsdoc.php?board=<?php echo $brd_encode; ?>"><?php  echo htmlspecialchars($brdarr["DESC"]); ?></a>&gt;</li>
<?php
}
?>
</ul></td>
              </tr>
		</table>
<?php
}

function gen_sec_hot_subjects_html($secid)
{
	# load xml doc
	$boardrank_file = BBS_HOME . sprintf("/xml/day_sec%d.xml", $secid);
	$doc = domxml_open_file($boardrank_file);
	if (!$doc)
		return;


	$root = $doc->document_element();
	$boards = $root->child_nodes();
?>
<ul style="margin: 10px 0px 0px 40px; padding: 0px;">
<?php
	$brdarr = array();
	# shift through the array
	while($board = array_shift($boards))
	{
	    if ($board->node_type() == XML_TEXT_NODE)
		continue;

	    $hot_title = find_content($board, "title");
	    $hot_author = find_content($board, "author");
	    $hot_board = find_content($board, "board");
	    $hot_time = find_content($board, "time");
	    $hot_number = find_content($board, "number");
	    $hot_groupid = find_content($board, "groupid");

		$brdnum = bbs_getboard($hot_board, $brdarr);
		if ($brdnum == 0)
			continue;
		$brd_encode = urlencode($brdarr["NAME"]);
?>
<li class="default"><a href="bbstcon.php?board=<?php echo $brd_encode; ?>&gid=<?php echo $hot_groupid; ?>"><?php echo htmlspecialchars($hot_title); ?> </a>&nbsp;&nbsp;&lt;<a href="bbsdoc.php?board=<?php echo $brd_encode; ?>"><?php  echo htmlspecialchars($brdarr["DESC"]); ?></a>&gt;</li>
<?php
	}
?>
</ul>
<?php
}

function gen_sections_html()
{
global $section_nums;
global $section_names;

# load xml doc
$boardrank_file = BBS_HOME . "/xml/board.xml";
$doc = domxml_open_file($boardrank_file);
	if (!$doc)
		return;


$root = $doc->document_element();
$boards = $root->child_nodes();

$sec_count = count($section_nums);
$sec_boards = array();
$sec_boards_num = array();
for ($i = 0; $i < $sec_count; $i++)
{
	$sec_boards[$i] = array();
	$sec_boards_num[$i] = 0;
}
$t = array(); // ������ű任��
for ($i = 0; $i < $sec_count - 2; $i++)
	$t[$i] = $i + 2;
$t[$i] = 0;
$t[$i+1] = 1;

# shift through the array
while($board = array_shift($boards))
{
    if ($board->node_type() == XML_TEXT_NODE)
        continue;

    $ename = find_content($board, "EnglishName");
    $cname = find_content($board, "ChineseName");
    $visittimes = find_content($board, "VisitTimes");
    $staytime = find_content($board, "StayTime");
    $secid = find_content($board, "SecId");
	$sec_boards[$secid][$sec_boards_num[$secid]]["EnglishName"] = $ename;
	$sec_boards[$secid][$sec_boards_num[$secid]]["ChineseName"] = $cname;
	$sec_boards[$secid][$sec_boards_num[$secid]]["VisitTimes"] = $visittimes;
	$sec_boards[$secid][$sec_boards_num[$secid]]["StayTime"] = $staytime;
	$sec_boards_num[$secid]++;
}
?>
	<table width="97%" border="0" cellpadding="0" cellspacing="0" class="type_title">
        <tr> 
		  <td width="23">&nbsp;</td>
          <td>&gt;&gt;���ྫ��������&gt;&gt;</td>
        </tr>
	</table>
		<table border="0" cellpadding="0" cellspacing="0" width="97%">
<?php
	for ($i = 0; $i < $sec_count; $i++)
	{
		if (defined("SITE_NEWSMTH") && ($t[$i]==0 || $t[$i]==1)) continue;
?>
<tr> 
  <td valign="top" class="MainContentText" style="padding-top: 5px;"> 
��<strong>[<a href="bbsboa.php?group=<?php echo $t[$i]; ?>"><?php echo htmlspecialchars($section_names[$t[$i]][0]); ?></a>]</strong>&nbsp;&nbsp;
<?php
		$brd_count = $sec_boards_num[$t[$i]] > 5 ? 5 : $sec_boards_num[$t[$i]];
		for ($k = 0; $k < $brd_count; $k++)
		{
?>
<a href="bbsdoc.php?board=<?php echo urlencode($sec_boards[$t[$i]][$k]["EnglishName"]); ?>"><?php echo $sec_boards[$t[$i]][$k]["ChineseName"]; ?></a>, 
<?php
		}
?>
<a href="bbsboa.php?group=<?php echo $t[$i]; ?>">����&gt;&gt;</a>
<?php
		gen_sec_hot_subjects_html($t[$i]);
?>
</td>
</tr>
<?php
		if ($sec_count - $i > 1)
		{
?>
        <tr> 
          <td height="1" bgcolor="CCCCCC"></td>
        </tr>
<?php
		}
	}
?>
      </table>
<?php
}
function gen_system_vote_html()
{
$vote_file = BBS_HOME."/vote/sysvote.html";
if(!file_exists($vote_file)) return;
include($vote_file);
?>
<br />	
<?php
}

function gen_new_boards_html()
{
# load xml doc
$newboard_file = BBS_HOME . "/xml/newboards.xml";
$doc = domxml_open_file($newboard_file);
	if (!$doc)
		return;


$root = $doc->document_element();
$boards = $root->child_nodes();
	if (defined("SITE_NEWSMTH")) {
?>
      <table width="100%" height="18" border="0" cellpadding="0" cellspacing="0" class="helpert">
        <tr>
          <td width="100%"><a href="http://gceclub.sun.com.cn/chinese_java_docs.html" target="_blank"><img src="/guangg/200510/sun/sdn_china_logo.gif" border="0" /></a></td>
        </tr>
          <td>&nbsp;</td>
        <tr>
          <td width="100%"><a href="http://gceclub.sun.com.cn/download.html" target="_blank">Java API�ĵ����İ�</a></td>
        </tr>
          <td>&nbsp;</td>
      </table>
<?php } ?>
      <table width="100%" border="0" cellpadding="0" cellspacing="0" class="helpert">
        <tr> 
          <td class="helpert_left"></td>
          <td class="helpert_middle">�¿�����</td>
          <td class="helpert_right"></td>
          <td>&nbsp;</td>
        </tr>
      </table>
      <table width="100%" border="0" cellpadding="0" cellspacing="0" class="helper">
              <tr> 
                <td width="100%" class="MainContentText">
<ul style="margin: 5px 0px 0px 15px; padding: 0px;">
<?php
	$brdarr = array();
	$j = 0;
	# shift through the array
	while($board = array_shift($boards))
	{
		if($j > 9)
			break;
		
		if ($board->node_type() == XML_TEXT_NODE)
			continue;

		$ename = find_content($board, "filename"); // EnglishName
		$brdnum = bbs_getboard($ename, $brdarr);
		if ($brdnum == 0)
			continue;
		$brd_encode = urlencode($brdarr["NAME"]);
		$j ++ ;
?>
<li class="default">&lt;<a href="bbsdoc.php?board=<?php echo $brd_encode; ?>"><?php echo htmlspecialchars($brdarr["DESC"]); ?></a>&gt;</li>
<?php
	}
?>
</ul>
<?php
	if($j > 9)
	{
?>
<p align="right"><a href="bbsnewbrd.php">&gt;&gt;����</a></p>
<?php
	}
?>
	</td></tr>
      </table>
      <br>
<?php
}

function gen_recommend_boards_html()
{
# load xml doc
$boardrank_file = BBS_HOME . "/xml/rcmdbrd.xml";
$doc = domxml_open_file($boardrank_file);
	if (!$doc)
		return;


$root = $doc->document_element();
$boards = $root->child_nodes();
?>
      <table width="100%" border="0" cellpadding="0" cellspacing="0" class="helpert">
        <tr> 
          <td class="helpert_left"></td>
          <td class="helpert_middle">�Ƽ�����</td>
          <td class="helpert_right"></td>
          <td>&nbsp;</td>
        </tr>
      </table>
      <table width="100%" border="0" cellpadding="0" cellspacing="0" class="helper">
              <tr> 
                <td width="100%" class="MainContentText">
<ul style="margin: 5px 0px 0px 15px; padding: 0px;">
<?php
	$brdarr = array();
	# shift through the array
	while($board = array_shift($boards))
	{
		if ($board->node_type() == XML_TEXT_NODE)
			continue;

		$ename = find_content($board, "EnglishName");
		$brdnum = bbs_getboard($ename, $brdarr);
		if ($brdnum == 0)
			continue;
		$brd_encode = urlencode($brdarr["NAME"]);
?>
<li class="default">&lt;<a href="bbsdoc.php?board=<?php echo $brd_encode; ?>"><?php echo htmlspecialchars($brdarr["DESC"]); ?></a>&gt;</li>
<?php
	}
?>
</ul></td></tr>
      </table>
      <br>
<?php
}

function gen_commend_html()
{
# load xml doc
$commend_file = BBS_HOME . "/xml/commend.xml";
if( ! file_exists($commend_file) )
	return;
$doc = domxml_open_file($commend_file);
	if (!$doc)
		return;

$root = $doc->document_element();
$boards = $root->child_nodes();


$brdarr = array();
?>
	<table width="97%" border="0" cellpadding="0" cellspacing="0" class="recommend_title">
        <tr> 
		  <td width="23">&nbsp;</td>
          <td>&gt;&gt;�Ƽ�����&gt;&gt;</td>
        </tr>
	</table>

	<table border="0" cellpadding="0" cellspacing="0" width="97%">
	<tr><td height=10></td></tr>
<?php
# shift through the array
while($board = array_shift($boards))
{
    if ($board->node_type() == XML_TEXT_NODE)
        continue;

    $commend_title = find_content($board, "title");
    $commend_author = find_content($board, "author");
    $commend_o_board = find_content($board, "o_board");
    $commend_o_id = find_content($board, "o_id");
    $commend_id = find_content($board, "id");
    $commend_o_groupid = find_content($board, "o_groupid");
    $commend_brief = find_content($board, "brief");

	$brdnum = bbs_getboard($commend_o_board, $brdarr);
	if ($brdnum == 0)
		continue;
	$brd_encode = urlencode($brdarr["NAME"]);

?>
<tr>
<td valign="top" class="MainContentText"><li class=rec><a href="bbsrecon.php?id=<?php echo $commend_id;?>"><?php echo htmlspecialchars($commend_title);?> </a>&nbsp;&lt;<a href="bbsdoc.php?board=<?php echo $brd_encode;?>"><?php echo htmlspecialchars($brdarr["DESC"]);?></a>&gt;
&nbsp;&nbsp;[<a href="bbstcon.php?board=<?php echo $brd_encode;?>&gid=<?php echo $commend_o_groupid;?>">ͬ�����Ķ�ԭ��ԭ��</a>]
<dl style="MARGIN-TOP: 1px;MARGIN-BOTTOM: 5px; MARGIN-LEFT: 25px;"><dt>
<?php echo htmlspecialchars($commend_brief);?>
</dl>
</td>
</tr>
<?php
}
?>
<tr><td width="100%" height=15 align="right"><a href="bbsrecommend.php">>>>�����Ƽ�����>>></a></td></tr>
</table>
<?php
}

function gen_board_rank_html()
{
# load xml doc
$boardrank_file = BBS_HOME . "/xml/board.xml";
$doc = domxml_open_file($boardrank_file);
	if (!$doc)
		return;


$root = $doc->document_element();
$boards = $root->child_nodes();

?>
      <table width="100%" border="0" cellpadding="0" cellspacing="0" class="helpert">
        <tr> 
          <td class="helpert_left"></td>
          <td class="helpert_middle">��������</td>
          <td class="helpert_right"></td>
          <td>&nbsp;</td>
        </tr>
      </table>
      <table width="100%" border="0" cellpadding="0" cellspacing="0" class="helper">
<?php
$i = 0;
# shift through the array
while($board = array_shift($boards))
{
	if ($board->node_type() == XML_TEXT_NODE)
		continue;

	$ename = find_content($board, "EnglishName");
	$cname = find_content($board, "ChineseName");
?>
              <tr> 
                <td height="20" class="MainContentText"><?php echo $i+1; ?>. <a href="bbsdoc.php?board=<?php echo urlencode($ename); ?>"><?php echo htmlspecialchars($cname); ?></a></td>
              </tr>
<?php
	$i++;
	if ($i == 10)
		break;
}
?>
      </table>
	  <br>
<?php
}

function gen_blessing_list_html()
{
# load xml doc
$hotsubject_file = BBS_HOME . "/xml/bless.xml";
$doc = domxml_open_file($hotsubject_file);
	if (!$doc)
		return;

$root = $doc->document_element();
$boards = $root->child_nodes();

?>
<a name="todaybless"></a>
      <table width="100%" border="0" cellpadding="0" cellspacing="0" class="helpert">
        <tr> 
          <td class="helpert_left"></td>
          <td class="helpert_middle">����ף��</td>
          <td class="helpert_right"></td>
          <td>&nbsp;</td>
        </tr>
      </table>
      <table width="100%" border="0" cellpadding="0" cellspacing="0" class="helper">
<tr> 
<td class="MainContentText">
<ul style="margin: 5px 0px 0px 15px; padding: 0px;">
<?php
# shift through the array
while($board = array_shift($boards))
{
    if ($board->node_type() == XML_TEXT_NODE)
        continue;

    $hot_title = find_content($board, "title");
    $hot_board = find_content($board, "board");
    $hot_groupid = find_content($board, "groupid");
?>
<li class="default"><a href="bbstcon.php?board=<?php echo $hot_board; ?>&gid=<?php echo $hot_groupid; ?>"><?php echo htmlspecialchars($hot_title); ?> </a></li>
<?php
}
?>
</ul></td></tr>
      </table>
<?php
}
?>
<html>
<head>
<title></title>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<script src="www2-main.js"></script>
<script type="text/javascript">writeCssMainpage();</script>
</head>
<body leftmargin="5" topmargin="0" marginwidth="0" marginheight="0">
<?php if (defined("SITE_NEWSMTH")) { ?>
<script src="images/randomad.js" type="text/javascript"></script>
<table><tr><td valign=top width=154><img id="bottom"  src="images/ad/1.gif" onload=notifyLoad(this.src); name=bottom></td></tr></table>
<?php } else { ?>
<center style="padding: 0.5em;font-weight:bold;font-size:150%;"><?php echo BBS_FULL_NAME; ?></center>
<?php } ?>
<table border="0" cellpadding="0" cellspacing="0" width="100%">
  <form action="bbssel.php">
  <tr> 
    <td height="18" width="84" class="header" align="center"><a href="bbsdoc.php?board=Announce">ϵͳ����</a></td>
    <td width="84" class="header" align="center"><a href="bbsrecommend.php">�Ƽ�����</a></td>
    <td width="100" class="header" align="center"><a href="bbssec.php">����������</a></td>
    <td width="80" class="header" align="center"><a href="bbsrecbrd.php">�Ƽ�����</a></td>
    <td width="81" class="header" align="center"><a href="bbsbrdran.php">��������</a></td>
    <td width="79" class="header" align="center"><a href="#todaybless">����ף��</a></td>
    <td width="79" class="header" align="center">
<?php
	if(defined("HAVE_PC"))
	{
?>
    <a href="/pc/pcmain.php">ˮľBLOG</a>
<?php
	}
?>
    </td>
    <td class="header"></td>
    <td class="header" align="right" width="315"> <input type="text" name="board" size="12" maxlength="30" value="��������" class="text"> 
      <input type="submit" size="15" value="GO" class="button"> 
    </td>
  </tr></form>
</table>
<table border="0" cellpadding="0" cellspacing="0" width="100%">
  <tr>
    <td colspan="5" height="8"></td>
  </tr>
  <tr>
    <td width="80%">
<?php
	gen_commend_html();
?>
<br>
<?php
	gen_hot_subjects_html();
	gen_sections_html();
?>
</td>
    <td width="1" bgcolor="0066CC"></td>
    <td width="18">&nbsp;</td>
    <td align="left" valign="top"> 
<!-- ϵͳ���濪ʼ ��ʱ���ε�
      <table width="150" border="0" cellpadding="0" cellspacing="0" class="helpert">
        <tr> 
          <td class="helpert_left">&nbsp;</td>
          <td class="helpert_middle">ϵͳ����</td>
          <td class="helpert_right"></td>
          <td>&nbsp;</td>
        </tr>
      </table>
      <table width="150" border="0" cellpadding="0" cellspacing="0" class="helper">
        <tr> 
                <td height="20" class="MainContentText"><font color="#6E9E54"><img src="images/wen.gif" width="9" height="7"> 
                  ��վ��ͨ���ʷ��ʽӿ�</font></td>
              </tr>
              <tr> 
                <td height="20" class="MainContentText"><font color="#6E9E54"><img src="images/wen.gif" width="9" height="7"> 
                  �°�WEB��������</font></td>
              </tr>
              <tr> 
                <td height="20" class="MainContentText"><img src="images/wen.gif" width="9" height="7"></td>
              </tr>
              <tr> 
                <td height="20" class="MainContentText"><img src="images/wen.gif" width="9" height="7"></td>
              </tr>
              <tr> 
                <td height="20" class="MainContentText"><img src="images/wen.gif" width="9" height="7"></td>
        </tr>
      </table>
      <br>
ϵͳ�������   -->
<?php
	//gen_system_vote_html();
	gen_new_boards_html();
	gen_recommend_boards_html();
	gen_board_rank_html();
	gen_blessing_list_html();
?>
	  </td>
    <td width="10">&nbsp;</td>
  </tr>
</table>
<?php if (defined("SITE_NEWSMTH")) { ?>
<hr class="smth">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <tr>
    <td align="center" class="smth">��Ȩ���� <span style="font-family: Arial,sans-serif;">&copy;</span> <?php echo BBS_FULL_NAME; ?> 2005 [<a href="mailto:passed@263.net">������ϵ</a>]</td>
  </tr>
</table>
<?php } ?>
<br>
</body>
</html>
