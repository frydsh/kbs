<?php
require("site.php");
if (!bbs_ext_initialized())
	bbs_init_ext();

# iterate through an array of nodes
# looking for a text node
# return its content
function get_content($parent)
{
    $nodes = $parent->child_nodes();
    while($node = array_shift($nodes))
        if ($node->node_type() == XML_TEXT_NODE)
            return $node->node_value();
    return "";
}

# get the content of a particular node
function find_content($parent,$name)
{
    $nodes = $parent->child_nodes();
    while($node = array_shift($nodes))
        if ($node->node_name() == $name)
            return urldecode(get_content($node));
    return "";
}

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
	<table width="97%" border="0" cellpadding="0" cellspacing="0" background="images/lan1.gif" class="title">
        <tr> 
		  <td width="23">&nbsp;</td>
          <td>&gt;&gt;�����ȵ㻰������&gt;&gt;</td>
        </tr>
	</table>
	<table border="0" cellpadding="0" cellspacing="0" width="97%">
              <tr> 
                <td class="MainContentText">
<ul style="margin-top: 15px; margin-left: 25px">
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
<a href="/bbstcon.php?board=<?php echo $brd_encode; ?>&gid=<?php echo $hot_groupid; ?>"><?php echo htmlspecialchars($hot_title); ?></a> &nbsp;&nbsp;[����: <a href="/bbsqry.php?userid=<?php echo $hot_author; ?>"><?php  echo $hot_author; ?></a>]&nbsp;&nbsp;&lt;<a href="/bbsdoc.php?board=<?php echo $brd_encode; ?>"><?php  echo htmlspecialchars($brdarr["DESC"]); ?></a>&gt;</li>
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
<ul style="margin-top: 15px; margin-left: 25px">
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
<li class="default"><a href="/bbstcon.php?board=<?php echo $brd_encode; ?>&gid=<?php echo $hot_groupid; ?>"><?php echo htmlspecialchars($hot_title); ?></a>&nbsp;&nbsp;&lt;<a href="/bbsdoc.php?board=<?php echo $brd_encode; ?>"><?php  echo htmlspecialchars($brdarr["DESC"]); ?></a>&gt;</li>
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
	<table width="97%" border="0" cellpadding="0" cellspacing="0" background="images/lan3.gif" class="title">
        <tr> 
		  <td width="23">&nbsp;</td>
          <td>&gt;&gt;���ྫ��������&gt;&gt;</td>
        </tr>
	</table>
		<table border="0" cellpadding="0" cellspacing="0" width="97%">
<?php
	for ($i = 0; $i < $sec_count; $i++)
	{
?>
<tr> 
  <td valign="top" class="MainContentText"> 
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
?>
      <table width="100%" height="18" border="0" cellpadding="0" cellspacing="0" class="helpert">
        <tr> 
          <td width="16" background="images/lt.gif">&nbsp;</td>
          <td width="66" bgcolor="#0066CC">�¿�����</td>
          <td width="16" background="images/rt.gif"></td>
          <td>&nbsp;</td>
        </tr>
      </table>
      <table width="100%" border="0" cellpadding="0" cellspacing="0" class="helper">
              <tr> 
                <td width="100%" class="MainContentText">
<ul style="margin-top: 5px; margin-left: 20px">
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
<p align="right"><a href="/bbsnewbrd.php">&gt;&gt;����</a></p>
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
      <table width="100%" height="18" border="0" cellpadding="0" cellspacing="0" class="helpert">
        <tr> 
          <td width="16" background="images/lt.gif">&nbsp;</td>
          <td width="66" bgcolor="#0066CC">�Ƽ�����</td>
          <td width="16" background="images/rt.gif"></td>
          <td>&nbsp;</td>
        </tr>
      </table>
      <table width="100%" border="0" cellpadding="0" cellspacing="0" class="helper">
              <tr> 
                <td width="100%" class="MainContentText">
<ul style="margin-top: 5px; margin-left: 20px">
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
	<table width="97%" border="0" cellpadding="0" cellspacing="0" background="images/lan2.gif" class="title">
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
<td valign="top" class="MainContentText"><LI class=default><a href="/bbsrecon.php?id=<?php echo $commend_id;?>"><?php echo htmlspecialchars($commend_title);?></a>&nbsp;&lt;<a href="/bbsdoc.php?board=<?php echo $brd_encode;?>"><?php echo htmlspecialchars($brdarr["DESC"]);?></a>&gt;
&nbsp;&nbsp;[<a href="/bbstcon.php?board=<?php echo $brd_encode;?>&gid=<?php echo $commend_o_groupid;?>">ͬ�����Ķ�ԭ��ԭ��</a>]
<dl style="MARGIN-TOP: 1px;MARGIN-BOTTOM: 5px; MARGIN-LEFT: 25px;"><dt>
<?php echo htmlspecialchars($commend_brief);?>
</dl>
</td>
</tr>
<?php
}
?>
<tr><td width="100%" height=15 align="right"><a href="/bbsrecommend.php">>>>�����Ƽ�����>>></a></td></tr>
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
      <table width="100%" height="18" border="0" cellpadding="0" cellspacing="0" class="helpert">
        <tr> 
          <td width="16" background="images/lt.gif">&nbsp;</td>
          <td width="66" bgcolor="#0066CC">��������</td>
          <td width="16" background="images/rt.gif"></td>
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
<a name="#todaybless">
      <table width="100%" height="18" border="0" cellpadding="0" cellspacing="0" class="helpert">
        <tr> 
          <td width="16" background="images/lt.gif">&nbsp;</td>
          <td width="66" bgcolor="#0066CC">����ף��</td>
          <td width="16" background="images/rt.gif"></td>
          <td>&nbsp;</td>
        </tr>
      </table>
      <table width="100%" border="0" cellpadding="0" cellspacing="0" class="helper">
<tr> 
<td class="MainContentText">
<ul style="margin-top: 5px; margin-left: 20px">
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
<li class="default"><a href="/bbstcon.php?board=<?php echo $hot_board; ?>&gid=<?php echo $hot_groupid; ?>"><?php echo htmlspecialchars($hot_title); ?></a></li>
<?php
}
?>
</ul></td></tr>
      </table>
	  <br>
<?php
}
?>
<html>
<head>
<title></title>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<link href="mainpage.css" rel="stylesheet" type="text/css">
</head>
<body leftmargin="5" topmargin="0" marginwidth="0" marginheight="0">
<table border="0" cellpadding="0" cellspacing="0" width="100%">
  <tr> 
    <td colspan="2" height="77"><img src="images/logo.gif" width="144" height="71"></td>
    <td colspan="7" >
    <object classid="clsid:D27CDB6E-AE6D-11cf-96B8-444553540000" codebase="http://download.macromedia.com/pub/shockwave/cabs/flash/swflash.cab#version=6,0,29,0" width="100%" height="100%">
      <param name="movie" value="ad.swf">
      <param name="quality" value="high">
      <embed src="ad.swf" quality="high" pluginspage="http://www.macromedia.com/go/getflashplayer" type="application/x-shockwave-flash"></embed>
    </object>
    </td>
  </tr>
  <form action="/cgi-bin/bbs/bbssel">
  <tr> 
    <td height="18" width="84" class="header" align="center">ϵͳ����</td>
    <td width="84" class="header" align="center"><a href="/bbsrecommend.php">�Ƽ�����</a></td>
    <td width="80" class="header" align="center"><a href="/bbssec.php">����������</a></td>
    <td width="80" class="header" align="center"><a href="/bbsrecbrd.php">�Ƽ�����</a></td>
    <td width="81" class="header" align="center"><a href="/bbsbrdran.php">��������</a></td>
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
    </td></form>
  </tr>
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
      <table width="150" height="18" border="0" cellpadding="0" cellspacing="0" class="helpert">
        <tr> 
          <td width="16" background="images/lt.gif">&nbsp;</td>
          <td width="66" bgcolor="#0066CC">ϵͳ����</td>
          <td width="16" background="images/rt.gif"></td>
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
<hr class="smth">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <tr>
    <td align="center" class="smth">��Ȩ���� &copy; <?php echo BBS_FULL_NAME; ?> 1995-2004 <a href="certificate.html">��ICP��02002��</a></td>
  </tr>
</table>
<br>
</body>
</html>
