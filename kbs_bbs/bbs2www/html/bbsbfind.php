<?php
    require("funcs.php");
    if ($loginok != 1)
		html_nologin();
    else
    {
        html_init("gb2312");

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
?>
<center>
<?php echo BBS_FULL_NAME; ?> -- ������������ [ʹ����: <?php echo $currentuser["userid"];?>]
<hr color="green"><br>
<?php
		if( !isset($_GET["submit"]) && !isset($_POST["submit"]) ){
?>
<table><form action="/bbsbfind.php" method=post>
<tr><td>��������: <input type="text" maxlength="24" size="24" name="board" value="<?php echo $brdarr["NAME"];?>"><br>
<tr><td>���⺬��: <input type="text" maxlength="50" size="20" name="title"> AND <input type="text" maxlength="50" size="20" name="title2">
<tr><td>���ⲻ��: <input type="text" maxlength="50" size="20" name="title3">
<tr><td>�����ʺ�: <input type="text" maxlength="12" size="12" name="userid"><br>
<tr><td>ʱ�䷶Χ: <input type="text" maxlength="4"  size="4"  name="dt" value="7"> ������<br>
<tr><td>��������:<input type="checkbox" name="mg">  ����������:<input type="checkbox" name="ag">  ��������:<input type="checkbox" name="og"><br><br>
<tr><td><input type="submit" name="submit" value="�ݽ���ѯ���">
</form></table>[<a href="/bbsdoc.php?board=<?php echo $brdarr["NAME"];?>">��������</a>]
</html>
<?php
			html_normal_quit();
		}

		if( isset( $_POST["title"] ) ){
			$title = $_POST["title"];
		}else
			$title="";

		if( isset( $_POST["title2"] ) ){
			$title2 = $_POST["title2"];
		}else
			$title2="";

		if( isset( $_POST["title3"] ) ){
			$title3 = $_POST["title3"];
		}else
			$title3="";

		if( isset( $_POST["userid"] ) ){
			$userid = $_POST["userid"];
		}else
			$userid="";

		if( isset( $_POST["dt"] ) ){
			$dt = $_POST["dt"];
		}else
			$dt=0;
		settype($dt, "integer");
		if($dt <= 0)
			$dt = 7;
		else if($dt > 9999)
			$dt = 9999;

		if( isset( $_POST["mg"] ) ){
			$mg = $_POST["mg"];
		}else
			$mg = "";
		if($mg != "")
			$mgon=1;
		else
			$mgon=0;

		if( isset( $_POST["og"] ) ){
			$og = $_POST["og"];
		}else
			$og = "";
		if($og != "")
			$ogon=1;
		else
			$ogon=0;

		if( isset( $_POST["ag"] ) ){
			$ag = $_POST["ag"];
		}else
			$ag = "";
		if($ag != "")
			$agon=1;
		else
			$agon=0;

		$articles = bbs_search_articles($board, $title, $title2, $title3, $userid, $dt, $mgon, $ogon, $agon);

		if( $articles <= 0 ){
			html_error_quit("ϵͳ����:".$articles);
		}
?>
����������'<?php echo $brdarr["NAME"];?>'��, ���⺬: '<?php echo htmlspecialchars($title);?>'<?php if($title2!="") echo " ��'".htmlspecialchars($title2)."'";?><?php if($title3!="") echo ",����'".htmlspecialchars($title3)."'";?> ����Ϊ: '<?php if($userid!="") echo $userid; else echo "������";?>', '<?php echo $dt;?>'�����ڵ�<?php if($mgon) echo "����"; if($agon) echo "����"; if($ogon) echo "����";?>����<br>
<table width=610>
<tr><td>���</td><td>���</td><td>����</td><td>����</td><td>����</td></tr>
<?php
		$i=0;
		foreach ($articles as $article)
		{
			$i++;
			$flags = $article["FLAGS"];
?>
<tr><td><?php echo $article["NUM"];?></td>
<td><?php echo $flags[0]; echo $flags[3];?></td>
<td><a href="/bbsqry.php?userid=<?php echo $article["OWNER"];?>"><?php echo $article["OWNER"];?></a></td>
<td><?php echo strftime("%b&nbsp;%e", $article["POSTTIME"]); ?></td>
<td><a href="/bbscon.php?board=<?php echo $brdarr["NAME"];?>&id=<?php echo $article["ID"];?>"><?php echo htmlspecialchars($article["TITLE"]); ?></a></td></tr>
<?php
		}
?>
</table>
<br>
���ҵ�<?php echo $i;?>ƪ���·�������<?php if($i>=999) echo "(ƥ��������, ʡ�Ե�1000�Ժ�Ĳ�ѯ���)";?>
<br>
[<a href="/bbsdoc.php?board=<?php echo $brdarr["NAME"];?>">���ر�������</a>] [<a href="javascript:history.go(-1)">������һҳ]</a>
<?php

		html_normal_quit();
    }
?>
