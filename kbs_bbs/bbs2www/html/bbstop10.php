<?php
	/**
	 * $Id$ 
	 */
	require("funcs.php");
	if ($loginok !=1 )
		html_nologin();
	else
	{
		$top_file = get_bbsfile("/etc/posts/day");
		$fp = fopen($top_file, "r");
		if ($fp == FALSE) {
		        html_init("gb2312");
			html_error_quit("�޷���ȡʮ��������");
                }
                $modifytime=filemtime($top_file);
                if (cache_header("public",$modifytime,300))
                	return;
	html_init("gb2312");
?>
<body>
<center><?php echo BBS_FULL_NAME; ?> -- ����ʮ�����Ż���
<hr>
<table border="1" width="610">
<tr><td>����</td><td>������</td><td>����</td><td>����</td><td>����</td></tr>
<?php
		fgets($fp, 256);
		fgets($fp, 256);
		for ($i = 0; $i < 10 && !feof($fp); $i++)
		{
			$one_line = fgets($fp, 256);
			if ($one_line == FALSE)
				break;
			$r_board = trim(substr($one_line, 41, 16));
			$r_num = trim(substr($one_line, 97, 4));
			$r_id = trim(substr($one_line, 118, 12));
			$one_line = fgets($fp, 256);
			if ($one_line == FALSE)
				break;
			$r_title = trim(substr($one_line, 27, 60));
?>
<tr>
<td>�� <?php echo $i+1; ?> ��</td>
<td><a href="/bbsdoc.php?board=<?php echo $r_board; ?>"><?php echo $r_board; ?></a></td>
<td><a href="/cgi-bin/bbs/bbstfind?board=<?php echo $r_board; ?>&title=<?php echo urlencode($r_title); ?>"><?php echo $r_title; ?></a></td>
<td><a href="/bbsqry.php?userid=<?php echo $r_id; ?>"><?php echo $r_id; ?></a></td>
<td><?php echo $r_num; ?></td>
</tr>
<?php
		}
		fclose($fp);
?>
</table></center>
<?php
		html_normal_quit();
	}
?>
