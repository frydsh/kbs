<?php
	/**
	 * $Id$ 
	 */
	require("funcs.php");
	if ($loginok !=1 )
		html_nologin();
	else
	{
                $usernum = $currentuser["index"];
		if (isset($_GET["board"]))
			$board = $_GET["board"];
		else 
			html_error_quit("�����������");
                $brdarr = array();
                $brdnum = bbs_getboard($board, $brdarr);
                if ($brdnum == 0)
			html_error_quit("�����������");
		if (bbs_checkreadperm($usernum,$brdnum)==0)
			html_error_quit("�����������");
		$top_file="vote/" . $board . "/notes";
		$fp = fopen($top_file, "r");
		if ($fp == FALSE) {
		        html_init("gb2312");
			html_error_quit("����û�б���¼");
                }
                fclose($fp);
                if (cache_header("public",filemtime($top_file),300))
                	return;
		html_init("gb2312");
?>
<body>
<center><?php echo $BBS_FULL_NAME; ?> -- ����¼ [������: <?php echo $board; ?>]<hr color=green>
<table border=1 width=610><tr><td><pre>
<?php
	bbs_printansifile($top_file);
?></tr></td>
</pre></table>
[<a href=bbsdoc.php?board=<?php echo $board; ?>>��������</a>]<?
    if (bbs_is_bm($brdnum,$usernum))
	echo "[<a href=bbsmnote.php?board=" . $board . ">�༭���滭��</a>]";
?> 
</center>
<?php
		html_normal_quit();
	}
?>
