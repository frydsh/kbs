<?php
	/**
	 * This file lists articles to user.
	 * $Id$
	 */
	require("funcs.php");

	function display_navigation_bar($brdarr,$brdnum,$start,$total,$order=FALSE)
	{
		global $section_names;
		$brd_encode = urlencode($brdarr["NAME"]);
	?>		
<table width="100%" border="0" cellspacing="0" cellpadding="0" class="b1">		
<form name="form1" action="<?php echo $_SERVER["PHP_SELF"]; ?>" method="get">
<input type="hidden" name="board" value="<?php echo $brdarr["NAME"]; ?>"/>
<tr>
<td>
    	<?php
    		if (strcmp($currentuser["userid"], "guest") != 0)
		{
    	?>
<a href="bbspst.php?board=<?php echo $brd_encode; ?>"><img src="images/postnew.gif" border="0" alt="������"></a>
    	<?php
    		}
    	?>
</td>
<td align="right">
 	<?php
		      if($order)
		      {
		   	if ($start <= $total - 20)
			{
		    ?>
[<a class="b1" href="<?php echo $_SERVER["PHP_SELF"]; ?>?board=<?php echo $brd_encode; ?>">��һҳ</a>]
[<a class="b1" href="<?php echo $_SERVER["PHP_SELF"]; ?>?board=<?php echo $brd_encode; ?>&start=<?php echo $start + 20; ?>">��һҳ</a>]
		    <?php
			}
			else
			{
		    ?>
[��һҳ] 
[��һҳ]
		    <?php
			}
			if ($start > 1)
			{
		?>
[<a class="b1" href="<?php echo $_SERVER["PHP_SELF"]; ?>?board=<?php echo $brd_encode; ?>&start=<?php if($start>20) echo $start - 20; else echo "1";?>">��һҳ</a>]
[<a class="b1" href="<?php echo $_SERVER["PHP_SELF"]; ?>?board=<?php echo $brd_encode; ?>&start=1">���һҳ</a>]
		    <?php
			}
			else
			{
		    ?>
[��һҳ] 
[���һҳ]
		    <?php
			}
		     }
		     else
		     {	
		     	if ($start > 1)
			{
		?>
[<a class="b1" href="<?php echo $_SERVER["PHP_SELF"]; ?>?board=<?php echo $brd_encode; ?>&start=1">��һҳ</a>]
[<a class="b1" href="<?php echo $_SERVER["PHP_SELF"]; ?>?board=<?php echo $brd_encode; ?>&start=<?php if($start > 20) echo $start - 20; else echo "1"; ?>">��һҳ</a>]
		    <?php
			}
			else
			{
		    ?>
[��һҳ] 
[��һҳ]
		    <?php
			}
			if ($start <= $total - 20)
			{
		    ?>
[<a class="b1" href="<?php echo $_SERVER["PHP_SELF"]; ?>?board=<?php echo $brd_encode; ?>&start=<?php echo $start + 20; ?>">��һҳ</a>]
[<a class="b1" href="<?php echo $_SERVER["PHP_SELF"]; ?>?board=<?php echo $brd_encode; ?>">���һҳ</a>]
		    <?php
			}
			else
			{
		    ?>
[��һҳ] 
[���һҳ]
		    <?php
			}
		    }
	?>
<input type="submit" class="b5" value="��ת��"/> �� <input type="text" name="start" size="3"  onmouseover=this.focus() onfocus=this.select() class="b5"> ƪ 
</td></tr></form></table>
	<?php
	}
	
	
	function display_g_articles($brdarr,$articles,$start,$order=FALSE){
?>
<table width="100%" border="0" cellspacing="0" cellpadding="3" class="t1">
<tr><td class="t2" width="40">���</td><td class="t2" width="30">���</td><td class="t2" width="85">����</td><td class="t2" width="50">����</td><td class="t2">����</td></tr>
<?php
		$brd_encode = urlencode($brdarr["NAME"]);
		$i = 0;
		foreach ($articles as $article)
		{
			$title = $article["TITLE"];
			if (strncmp($title, "Re: ", 4) != 0)
				$title = "�� " . $title;
			$flags = $article["FLAGS"];
?>
<tr>
<td class="t3"><?php echo $start + $i; ?></td>
<td class="t4">
<?php
			if ($flags[1] == 'y')
			{
				if ($flags[0] == ' ')
				{
?>
<font face="Webdings" color="#008000">&lt;</font>
<?php
				}
				else
				{
?>
<font color="#008000"><?php echo $flags[0]; ?></font>
<?php
				}
			}
			elseif ($flags[0] == 'N' || $flags[0] == '*')
			{
?>
<font color="#909090"><?php echo $flags[0]; ?></font>
<?php
			}
			else{
				if($flags[0]==' ')
					echo "&nbsp;";
				else
					echo $flags[0];
			}
?>
</td>
<td class="t3"><a class="ts1" href="/bbsqry.php?userid=<?php echo $article["OWNER"]; ?>"><?php echo $article["OWNER"]; ?></a></td>
<td class="t4"><?php echo strftime("%b&nbsp;%e", $article["POSTTIME"]); ?></td>
<td class="t5">
<a class="ts2" href="/bbsgcon.php?board=<?php echo $brd_encode; ?>&file=<?php echo $article["FILENAME"]; ?>&num=<?php echo $start + $i; ?>"><?php echo htmlspecialchars($title); ?></a>
</td>
</tr>
<?php
			$i++;
		}
?>
</table>
<?php		
		}

	if ($loginok != 1)
		html_nologin();
	else
	{
		html_init("gb2312","","",1);
		if (isset($_GET["board"]))
			$board = $_GET["board"];
		else
			html_error_quit("�����������");
		// ����û��ܷ��Ķ��ð�
		$brdarr = array();
		$brdnum = bbs_getboard($board, $brdarr);
		if ($brdnum == 0)
			html_error_quit("�����������");
		$usernum = $currentuser["index"];
		if (bbs_checkreadperm($usernum, $brdnum) == 0)
			html_error_quit("�����������");
		$total = bbs_countarticles($brdnum, $dir_modes["DIGEST"]);
		if ($total <= 0)
			html_error_quit("��������Ŀǰû������");
		bbs_set_onboard($brdnum,1);

		if (isset($_GET["start"]))
			$start = $_GET["start"];
		elseif (isset($_POST["start"]))
			$start = $_POST["start"];
		else
			$start = 0;
		settype($start, "integer");
		$artcnt = 20;
		/*
		 * �������һ��ʱ�������⣬���ܻᵼ����ű��ҡ�
		 * ԭ���������ε��� bbs_countarticles() �� bbs_getarticles()��
		 */
		if ($start == 0 || $start > ($total - $artcnt + 1))
			$start = ($total - $artcnt + 1);
		if ($start < 0)
			$start = 1;
		$articles = bbs_getarticles($board, $start, $artcnt, $dir_modes["DIGEST"]);
		if ($articles == FALSE)
			html_error_quit("��ȡ�����б�ʧ��");
		$bms = explode(" ", trim($brdarr["BM"]));
		$bm_url = "";
		if (strlen($bms[0]) == 0 || $bms[0][0] <= chr(32))
			$bm_url = "����������";
		else
		{
			if (!ctype_alpha($bms[0][0]))
				$bm_url = $bms[0];
			else
			{
				foreach ($bms as $bm)
				{
					$bm_url .= sprintf("<a href=\"/bbsqry.php?userid=%s\" class=\"b3\">%s</a> ", $bm, $bm);
				}
				$bm_url = trim($bm_url);
			}
		}
	
		$brd_encode = urlencode($brdarr["NAME"]);
?>
<body>
<a name="listtop"></a>
<table width="100%" border="0" cellspacing="0" cellpadding="3">
  <tr> 
    <td colspan="2" class="b2">
	    <a href="mainpage.html" class="b2"><?php echo BBS_FULL_NAME; ?></a>
	    -
	    <?php
	    	$sec_index = get_secname_index($brdarr["SECNUM"]);
		if ($sec_index >= 0)
		{
	    ?>
		<a href="/bbsboa.php?group=<?php echo $sec_index; ?>" class="b2"><?php echo $section_names[$sec_index][0]; ?></a>
	    <?php
		}
	    ?>
	    -
	    <a href="/bbsdoc.php?board=<?php echo $brd_encode; ?>" class="b2"><?php echo $brdarr["NAME"]; ?>��</a>(<a href="bbsnot.php?board=<?php echo $brd_encode; ?>" class="b2">���滭��</a>|<a href="/bbsfav.php?bname=<?php echo $brd_encode; ?>&select=-1" class="b2">Ԥ������</a>)
	    -
	    ��ժ��
    </td>
  </tr>
  <tr> 
    <td colspan="2" align="center" class="b4"><?php echo $brdarr["NAME"]."(".$brdarr["DESC"].")"; ?> ��</td>
  </tr>
  <tr><td class="b1">
  <img src="images/bm.gif" alt="����" align="absmiddle">���� <?php echo $bm_url; ?>
  </td>
    <td align="right" class="b1">
	    <img src="images/gmode.gif" align="absmiddle" alt="һ��ģʽ"><a class="b1" href="bbsdoc.php?board=<?php echo $brdarr["NAME"]; ?>">һ��ģʽ</a> 
	    <?php
                    if ($ann_path = bbs_getannpath($brdarr["NAME"])){
	    ?>
	    | 
  	    <img src="images/soul.gif" align="absmiddle" alt="������"><a class="b1" href="/cgi-bin/bbs/bbs0an?path=<?php echo urlencode($ann_path); ?>">������</a>
	    <?php
		}
	    ?>
	    | 
  	    <img src="images/search.gif" align="absmiddle" alt="���ڲ�ѯ"><a class="b1" href="/bbsbfind.php?board=<?php echo $brd_encode; ?>">���ڲ�ѯ</a>
	    <?php
    		if (strcmp($currentuser["userid"], "guest") != 0)
		{
    	    ?>
	    | 
  	    <img src="images/vote.gif" align="absmiddle" alt="����ͶƱ"><a class="b1" href="/bbsshowvote.php?board=<?php echo $brd_encode; ?>">����ͶƱ</a>
	    | 
  	    <img src="images/model.gif" align="absmiddle" alt="����ģ��"><a class="b1" href="/bbsshowtmpl.php?board=<?php echo $brd_encode; ?>">����ģ��</a>
    	    <?php
    		}
    	    ?>	
    </td>
  </tr>
  <tr> 
    <td colspan="2" height="9" background="images/dashed.gif"> </td>
  </tr>
  <tr><td colspan="2" align="right" class="b1">
  <?php
  	display_navigation_bar($brdarr, $brdnum, $start, $total, $order_articles );
  ?>
  </td></tr>
  <tr> 
    <td colspan="2" align="center">
    	<?php
		display_g_articles($brdarr,$articles,$start,$order=FALSE);
	?>	
    </td>
  </tr>
  <tr><td colspan="2" align="right" class="b1">
  <?php
  	display_navigation_bar($brdarr, $brdnum, $start, $total, $order_articles);
  ?>
  </td></tr>
  <tr> 
    <td colspan="2" height="9" background="images/dashed.gif"> </td>
  </tr>
  <tr> 
    <td colspan="2" align="center" class="b1">
    	[<a href="#listtop">���ض���</a>]
    	[<a href="bbsdoc.php?board=<?php echo $brdarr["NAME"]; ?>">����һ��ģʽ</a>]
    	[<a href="javascript:location=location">ˢ��</a>]
  	    [<a href="/bbsbfind.php?board=<?php echo $brd_encode; ?>">���ڲ�ѯ</a>]
    	<?php
    		if (strcmp($currentuser["userid"], "guest") != 0)
		{
    	?>
    	[<a href="/cgi-bin/bbs/bbsclear?board=<?php echo $brd_encode; ?>&start=<?php echo $start; ?>">���δ��</a>]
	<?php
		}
		if (bbs_is_bm($brdnum, $usernum))
		{
	?>
	[<a href="bbsmdoc.php?board=<?php echo $brd_encode; ?>">����ģʽ</a>]
	<?php
		}
	?>
    </td>
  </tr>
</table>
<?php
	}
	html_normal_quit();
?>
