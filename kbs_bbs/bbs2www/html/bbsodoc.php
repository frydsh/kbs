<?php
	/**
	 * This file lists original articles to user.
	 * $Id$
	 */
	require("funcs.php");
	if ($loginok != 1)
		html_nologin();
	else
	{
		html_init("gb2312");
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
		bbs_set_onboard($brcnum,1);

		$total = bbs_countarticles($brdnum, $dir_modes["ORIGIN"]);
		if ($total <= 0)
			html_error_quit("��������Ŀǰû������");
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
		$articles = bbs_getarticles($board, $start, $artcnt, $dir_modes["ORIGIN"]);
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
					$bm_url .= sprintf("<a href=\"/bbsqry.php?userid=%s\">%s</a> ", $bm, $bm);
				}
				$bm_url = trim($bm_url);
			}
		}
?>
<body>
<nobr/>
<center><?php echo BBS_FULL_NAME; ?> -- [������: <?php echo $brdarr["NAME"]; ?>] ����[<?php echo $bm_url; ?>] ������[<?php echo $total; ?>] <a href="/cgi-bin/bbs/bbsbrdadd?board=<?php echo $brdarr["NAME"]; ?>">Ԥ������</a>
<hr class="default"/>
<table width="613">
<tr><td>���</td><td>���</td><td>����</td><td>����</td><td>����</td></tr>
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
<td><?php echo $start + $i; ?></td>
<td>
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
			else
				echo $flags[0];
?>
</td>
<td><a href="/bbsqry.php?userid=<?php echo $article["OWNER"]; ?>"><?php echo $article["OWNER"]; ?></a></td>
<td><?php echo strftime("%b&nbsp;%e", $article["POSTTIME"]); ?></td>
<td>
<a href="/cgi-bin/bbs/bbstcon?board=<?php echo $brd_encode; ?>&gid=<?php echo $article["GROUPID"]; ?>"><?php echo htmlspecialchars($title); ?></a>
</td>
</tr>
<?php
			$i++;
		}
?>
</table>
<hr class="default"/>
<a href="bbspst.php?board=<?php echo $brd_encode; ?>">��������</a>
<a href="javascript:location=location">ˢ��</a>
<?php
		if ($start > 0)
		{
?>
<a href="<?php echo $_SERVER["PHP_SELF"]; ?>?board=<?php echo $brd_encode; ?>&start=<?php echo $start - $artcnt; ?>">��һҳ</a>
<?php
		}
		if ($start < $total - 20)
		{
?>
<a href="<?php echo $_SERVER["PHP_SELF"]; ?>?board=<?php echo $brd_encode; ?>&start=<?php echo $start + $artcnt; ?>">��һҳ</a>
<?php
		}
		if (bbs_is_bm($brdnum, $usernum))
		{
?>
<a href="bbsmdoc.php?board=<?php echo $brd_encode; ?>">����ģʽ</a>
<?php
		}
?>
<a href="bbsnot.php?board=<?php echo $brd_encode; ?>">���滭��</a>
<a href="bbsdoc.php?board=<?php echo $brd_encode; ?>">��ͨģʽ</a>
<?php
		$ann_path = bbs_getannpath($brdarr["NAME"]);
		if ($ann_path != FALSE)
		{
?>
<a href="/cgi-bin/bbs/bbs0an?path=<?php echo urlencode($ann_path); ?>">������</a>
<?php
		}
?>
<a href="/bbsbfind.php?board=<?php echo $brd_encode; ?>">���ڲ�ѯ</a>
<?php
		if (strcmp($currentuser["userid"], "guest") != 0)
		{
?>
<a href="/cgi-bin/bbs/bbsclear?board=<?php echo $brd_encode; ?>&start=<?php echo $start; ?>">���δ��</a>
<?php
		}
		$sec_index = get_secname_index($brdarr["SECNUM"]);
		if ($sec_index >= 0)
		{
?>
<a href="bbsboa.php?group=<?php echo $sec_index; ?>">����[<?php echo $section_names[$sec_index][0]; ?>]</a>
<?php
		}
?>
<form name="form1" action="<?php echo $_SERVER["PHP_SELF"]; ?>" method="get">
<input type="hidden" name="board" value="<?php echo $brdarr["NAME"]; ?>"/>
<input type="submit" value="��ת��"/> �� <input type="text" name="start" size="4"/> ƪ
</form>
<?php
		html_normal_quit();
	}
?>
