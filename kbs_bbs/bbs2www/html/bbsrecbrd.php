<?php
	/**
	 * This file lists recommend boards to user.
	 * windinsn Apr 8 , 2004
	 */
	require("funcs.php");
	require("board.inc.php");
	function get_content($parent)
	{
	    $nodes = $parent->child_nodes();
	    while($node = array_shift($nodes))
	        if ($node->node_type() == XML_TEXT_NODE)
	            return $node->node_value();
	    return "";
	}
	function find_content($parent,$name)
	{
	    $nodes = $parent->child_nodes();
	    while($node = array_shift($nodes))
	        if ($node->node_name() == $name)
	            return urldecode(get_content($node));
	    return "";
	}

	if ($loginok !=1 )
		html_nologin();
	else
	{
		$boardrank_file = BBS_HOME . "/xml/rcmdbrd.xml";
		if (cache_header("public, must-revalidate",filemtime($boardrank_file),3600))
               		return;
		$doc = domxml_open_file($boardrank_file);
		if (!$doc)
			html_error_quit("Ŀǰ�����Ƽ�������");
			
		$root = $doc->document_element();
		$boards = $root->child_nodes();
		$brdarr = array();
		
		html_init("gb2312");
		
?>
<body topmargin="0">
<table width="100%" border="0" cellspacing="0" cellpadding="3" >
  <tr> 
    <td colspan="2" class="kb2" colspan=2>
	    <a class="kts1"  href="mainpage.php"><?php echo BBS_FULL_NAME; ?></a>  - <a class="kts1"  href="bbssec.php">����������</a> - [�Ƽ�������]    </td>
  </tr>
   <tr valign=bottom align=center> 
    <td align="left" class="kb4">&nbsp;&nbsp;&nbsp;&nbsp; �Ƽ�������</td>
     <td align="right" class="kb1" >&nbsp;</td>
  </tr>
   <tr> 
    <td colspan="2" height="9" background="images/dashed.gif"> </td>
  </tr>
   <tr><td colspan="2" align="center">
    	<table width="100%" border="0" cellspacing="0" cellpadding="3" class="kt1">	
<form name="form1" action="bbsdoc.php" method="get">
<input type="hidden" name="board" value="Botany">
<tr>
<td class="kt2" width="5%"> </td>
<td class="kt2" width="2%"> </td>
<td class="kt2" width="24%">����������</td>
<td class="kt2" width="10%">���</td>
<td class="kt2" width="41%">��������</td>
<td class="kt2" width="18%">����</td>
</tr>
<?php
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
		$i ++ ;
?>
<tr>
<td class="kt3 c2" align=center height=25><?php echo $i; ?></td>
<td class="kt4 c1" >
	<img src="images/newgroup.gif" height="15" width="20" title="�Ƽ�������">
</td>
<td class="kt3 c1">
	<a class="kts1"  href="/bbsdoc.php?board=<?php echo $brd_encode; ?>"><?php echo $brdarr["NAME"]; ?></a>
</td>
<td class="kt3 c2" align="center">
	<?php echo $brdarr["CLASS"]; ?>
</td>
<td class="kt3 c1">
&nbsp;&nbsp;<a class="kts1" href="/bbsdoc.php?board=<?php echo $brd_encode; ?>"><?php echo $brdarr["DESC"]; ?></a>
</td>
<td class="kt3 c2" align="center">
<?php
			$bms = explode(" ", trim($brdarr["BM"]));
			if (strlen($bms[0]) == 0 || $bms[0][0] <= chr(32))
				echo "����������";
			else
			{
				if (!ctype_print($bms[0][0]))
					echo $bms[0];
				else
				{
?>
<a class="kts1" href="/bbsqry.php?userid=<?php echo $bms[0]; ?>"><?php echo $bms[0]; ?></a>
<?php
				}
			}
		} //end while
?>
</table>
   <tr> 
    <td colspan="2" height="9" background="images/dashed.gif"> </td>
  </tr>
  </table>
</center>
<?php
		bbs_boards_navigation_bar();
		html_normal_quit();
	}
?>
