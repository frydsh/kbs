<?php
	require("funcs.php");
	
	function show_file( $board , $brd_encode , $bid , $article , $articlenum)
	{
?>
&nbsp;<table width="90%" cellspacing=0 cellpadding=0 class="t1">
<tr><td class="t2">
<table width="100%" cellspacing=0 cellpadding=5 class=b7>
<tr><td>
[<a href="/bbscon.php?board=<?php echo $brd_encode; ?>&id=<?php echo $article["ID"]; ?>"><font class="b7">��ƪȫ��</font></a>]
[<a href="/bbspst.php?board=<?php echo $brd_encode; ?>&reid=<?php echo $article["ID"]; ?>"><font class="b7">�ظ�����</font></a>]
[<a href="/bbspstmail.php?board=<?php echo $brd_encode; ?>&file=<?php echo $article["FILENAME"]; ?>&userid=<?php echo $article["OWNER"]; ?>&title=<?php if(strncmp($article["TITLE"],"Re:",3)) echo "Re: ";  echo urlencode($article["TITLE"]); ?>"><font class="b7">���Ÿ�����</font></a>]
[��ƪ���ߣ�<a href="/bbsqry.php?userid=<?php echo $article["OWNER"]; ?>"><font class="b7"><?php echo $article["OWNER"]; ?></font></a>]
[<a href="/bbsdoc.php?board=<?php echo $brd_encode; ?>"><font class="b7">����������</font></a>]
[<a href="#top"><font class="b7">���ض���</font></a>]
</td>
<td align=center><strong><?php echo $articlenum; ?></strong>
</td></tr></table>
</td></tr>
<tr><td class="t8">
<font class="content">
<script language="Javascript" src="/jscon.php?bid=<?php echo $bid; ?>&id=<?php echo $article["ID"]; ?>">
</script>
</font>
</td></tr>
</table><br />
<?php		
		
	}
	
	function page_navigation( $brd_encode , $gid , $start , $totlepage , $pno )
	{
?>
��<strong>��ҳ��
<?php
		for( $j = 1 ; $j < $totlepage + 1; $j ++ )
		{
			if( $pno != $j  )
				echo "<a href=\"/bbstcon.php?board=".$brd_encode."&gid=".$gid."&start=".$start."&pno=".$j."\"><u>".$j."</u></a>\n";
			else
				echo $j."\n";
		}
?>
��</strong>
<?php		
	}
	
	
	$gid = $_GET["gid"];
	settype($gid, "integer");
	$start = $_GET["start"];
	settype($start, "integer");
	$pno = $_GET["pno"];
	settype($pno, "integer");
	
	if( $gid < 0 ) $gid = 0 ; 
	if($start <= 0 ) $start = $gid;
	$board = $_GET["board"];
	$brdarr = array();
	
	$bid = bbs_getboard($board , $brdarr);
	if($bid == 0)
	{
		html_init("gb2312","","",1);
		html_error_quit("�����������");
	}
	$board = $brdarr["NAME"];
	$board_desc = $brdarr["DESC"];
	$brd_encode = urlencode( $board );
	
	$isnormalboard = bbs_normalboard($board);
	if (($loginok != 1)&&!$isnormalboard) 
        {
        	html_nologin();
        	return;
        }
        bbs_set_onboard($brdnum,1);

	if($loginok == 1)
		$usernum = $currentuser["index"];
	if (!$isnormalboard && bbs_checkreadperm($usernum, $bid) == 0) 
	{
		html_init("gb2312","","",1);
		html_error_quit("�����������");
	}
	
	$haveprev = 0;
	$num = bbs_get_threads_from_gid($bid, $gid, $start , $articles , $haveprev );
	if( $num == 0 )
	{
		html_init("gb2312","","",1);
		html_error_quit("����Ĳ���");
	}
	
	$pagesize = 20;
	$totlepage = ( $num - 1 ) / $pagesize + 1;
	$totlepage = intval( $totlepage );
	if( $pno < 1 || $pno > $totlepage )
		$pno = 1;
	
	$startnum = ( $pno - 1 ) * $pagesize;
	$endnum = $startnum + $pagesize;
	if( $endnum > $num )
		$endnum = $num;
	/*
        ** Cacheֻ�ж���ͬ��������һƪ���£�û�й˼�ǰ�����µ��޸�  windinsn jan 26 , 2004
        */ 
        if ($isnormalboard)
	{
       		$lastfilename = bbs_get_board_filename($board , $articles[$num - 1]["FILENAME"]);
		if (cache_header("public",filemtime($lastfilename),300))
        	return;
        }
        
        html_init("gb2312","","",1);
?>
<body topmargin="0" leftmargin="0">
<a name="top">
<table width="100%" border="0" cellspacing="0" cellpadding="3">
  <tr> 
    <td class="b2">
	    <a href="/mainpage.html" class="b2"><font class="b2"><?php echo BBS_FULL_NAME; ?></font></a>
	    -
	    <?php
	    	$sec_index = get_secname_index($brdarr["SECNUM"]);
		if ($sec_index >= 0)
		{
	    ?>
		<a href="/bbsboa.php?group=<?php echo $sec_index; ?>" class="b2"><font class="b2"><?php echo $section_names[$sec_index][0]; ?></font></a>
	    <?php
		}
	    ?>
	    -
	    <a href="/bbsdoc.php?board=<?php echo $brd_encode; ?>" class="b2"><font class="b2"><?php echo $brdarr["NAME"]; ?></font></a>��(<a href="bbsnot.php?board=<?php echo $brd_encode; ?>" class="b2"><font class="b2">���滭��</font></a>|<a href="/bbsfav.php?bname=<?php echo $brd_encode; ?>&select=-1" class="b2"><font class="b2">��ӵ��ղؼ�</font></a>)
    </td>
  </tr>
  <tr>
    <td class="b4">ͬ�����Ķ���&nbsp;&nbsp;&nbsp<?php echo htmlspecialchars($articles[0]["TITLE"]); ?></td>
  </tr>
  <tr>
    <td class="b2"><?php page_navigation( $brd_encode , $gid , $start , $totlepage , $pno ); ?></td>
  </tr>
</table>
<?php
	for( $i = $startnum ; $i < $endnum ; $i ++ )
	{
		show_file( $board , $brd_encode , $bid , $articles[$i] , $i + 1);	
		if ($loginok==1&&($currentuser["userid"] != "guest"))
			bbs_brcaddread($board , $articles[$i]["ID"]);
	}
	
?>
<p align="left" class="b2">
<?php page_navigation( $brd_encode , $gid , $start , $totlepage , $pno ); ?>
</p>
<hr size=1>
<p align=center>
<a href="javascript:history.go(-1)">����</a>
<a href="/bbsdoc.php?board=<?php echo $board; ?>">����<strong><?php echo $board_desc; ?></strong>������</a>
<a href="#top">���ض���</a>
</p>
<?php
	html_normal_quit();
?>