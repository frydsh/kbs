<?php
	require("funcs.php");
	if ($loginok != 1)
		html_nologin();
	else
	{
		html_init("gb2312");
        if ($currentuser["userid"]=="guest")
            html_error_quit("�Ҵҹ��Ͳ���ת������");

		if( !isset($_GET["board"]) && !isset($_POST["board"]))
			html_error_quit("�����������");
		if( isset($_GET["board"]) )
			$board = $_GET["board"];
		else
			$board = $_POST["board"];

		$brdarr = array();
		$brdnum = bbs_getboard($board, $brdarr);
		if ($brdnum == 0){
			html_error_quit("�����������1");
		}
		$usernum = $currentuser["index"];
		if (bbs_checkreadperm($usernum, $brdnum) == 0){
			html_error_quit("�����������2");
		}

		if( !isset($_GET["id"]) && !isset($_POST["id"]))
			html_error_quit("��������º�");
		if( isset($_GET["id"]) )
			$id = $_GET["id"];
		else
			$id = $_POST["id"];

		$articles = array ();
		$num = bbs_get_records_from_id($brdarr["NAME"], $id, 
			$dir_modes["NORMAL"] , $articles);
		if($num == 0)
			html_error_quit("��������º�");
?>
<center>
<?php echo BBS_FULL_NAME; ?> -- ת������ [ʹ����: <?php echo $currentuser["userid"];?>]
<hr color="green"><br>
</center>
<?php
		if ( !isset($_POST["submit"])){
?>
���±���: <?php echo htmlspecialchars($articles[1]["TITLE"]);?><br>
��������: <?php echo $articles[1]["OWNER"];?><br>
ԭ������: <?php echo $brdarr["NAME"];?><br>
<form action="/bbsfwd.php" method="post">
<input type="hidden" name="board" value="<?php echo $brdarr["NAME"];?>">
<input type="hidden" name="id" value="<?php echo $id;?>">
������ת�ĸ� <input type="text" name="target" size="40" maxlength="69" value="<?php echo $currentuser["email"];?>"> (������Է���id��email��ַ). <br>
<input type="checkbox" name="big5" value="1"> ʹ��BIG5��<br>
<input type="checkbox" name="noansi" value="1" checked> ����ANSI���Ʒ�<br>
<input type="submit" name="submit" value="ȷ��ת��">
</form>
<?php
			html_normal_quit();
		}

		if( isset($_POST["target"]) )
			$target =  $_POST["target"];
		else
			$target = "";
		if($target == "")
			html_error_quit("��ָ������");

		if( isset($_POST["big5"]) )
			$big5 = $_POST["big5"];
		else
			$big5=0;
		settype($big5, "integer");

		if( isset($_POST["noansi"]) )
			$noansi = $_POST["noansi"];
		else
			$noansi=0;
		settype($noansi, "integer");

		$ret = bbs_doforward($brdarr["NAME"], $articles[1]["FILENAME"], $articles[1]["TITLE"], $target, $big5, $noansi);
		if($ret < 0)
			html_error_quit("ϵͳ����:".$ret);
?>
������ת�ĸ�'<?php echo $target;?>'<br>
[<a href="javascript:history.go(-2)">����</a>]
<?php
	}
?>
