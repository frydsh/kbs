<?php
	/**
	 * This file list all friends online.
	 * by binxun
	 * $Id$
	 */
	 
	require("funcs.php");
	
	if ($loginok != 1)
		html_nologin();
	else
	{
		html_init("gb2312");
	 	$friends = bbs_getonlinefriends();
		if ($friends == 0)
			$num = 0;
		else
			$num = count($friends);
?>
<center><?php echo BBS_FULL_NAME; ?> -- ���ߺ����б� [ʹ����: <?php echo $currentuser["userid"];?>]<hr>
<?php 
 	if ($num == 0) echo "Ŀǰû�к�������";
	else
	{
?>
<table border="1" width="610">
<tr><td>���<td>��<td>ʹ���ߴ���<td>ʹ�����ǳ�<td>����<td>��̬<td>����
<?php
		for($i = 0; $i < $num; $i++)
		{
			echo "<tr><td>" . ($i + 1);
			echo "<td>��";
			if($friends[$i]["invisible"]) echo "<font color=\"green\">C</font>";
			else
				echo " ";
?>
<td><a href="/bbsqry.php?userid=<?php echo $friends[$i]["userid"];?>"><?php echo $friends[$i]["userid"];?></a>
<td><a href="/bbsqry.php?userid=<?php echo $friends[$i]["userid"];?>"><?php echo htmlspecialchars($friends[$i]["username"]);?></a>
<td><?php echo $friends[$i]["userfrom"];?>
<?php
			if($friends[$i]["invisible"]) echo "<td>������...";
			else
				echo "<td>" . $friends[$i]["mode"];
			if($friends[$i]["idle"] == 0) echo "<td> ";
			else
				echo "<td>" . $friends[$i]["idle"];			
		}
		echo "</table>";
	}
?>
<hr>
[<a href="/bbsfall.php">ȫ����������</a>]
</center>
<?php
	html_normal_quit();
	}
?>
