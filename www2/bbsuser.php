<?php
	require("www2-funcs.php");

	if (defined("SITE_SMTH")) // ���ṩ�����û��б� add by windinsn, May 5,2004
		exit ();

	login_init();
	page_header("�����û��б�");
	
	if( isset( $_GET["start"] ) ){
		$start=$_GET["start"];
	} else {
		$start=1;
	}
	if ($start<=0) $start=1;
	$num=20;
	$users = bbs_getonline_user_list($start,$num);
	if ($users == 0)
		$count = 0;
	else
		$count = count($users);

?>
<table class="main adj">
<col class="center"/><col/><col/><col/><col/><col/><col class="right"/>
<tbody>
<tr><th>���</th><th>��</th><th>ʹ���ߴ���</th><th>ʹ�����ǳ�</th><th>����</th><th>��̬</th><th>����</th></tr>
<?php
		for($i = 0; $i < $count; $i++) {
			echo "<tr><td>" . ($i+$start) . "</td>";
			echo "<td>" . ($users[$i]["isfriend"]?"��" : "  ") . "</td>";
			echo "<td><a href=\"bbsqry.php?userid=" . $users[$i]["userid"] . "\">" . $users[$i]["userid"] . "</a></td>";
			echo "<td><a href=\"bbsqry.php?userid=" . $users[$i]["userid"] . "\">" . htmlspecialchars($users[$i]["username"]) . "</a></td>";
			echo "<td>" . $users[$i]["userfrom"] . "</td>";
			echo "<td>" . $users[$i]["mode"] . "</td>";
			echo "<td>" . ($users[$i]["idle"]!=0?$users[$i]["idle"]:" ") . "</td></tr>\n";
		}
?>
</tbody></table>
<div class="oper">
[<a href="bbsfriend.php">���ߺ���</a>]
<?php
	$prev = $start - $num;
	if ($prev <= 0) $prev = 1;
	if( $prev < $start ){
?>
[<a href="bbsuser.php?start=<?php echo $prev;?>">��һҳ</a>]
<?php
	}
	if( $count >= $num ){
?>
[<a href="bbsuser.php?start=<?php echo $start+$num;?>">��һҳ</a>]
<?php
	}
?>
</div>
<form method="GET">
<input type="submit" value="��ת����"> <input type="input" size="4" name="start"> ��ʹ����
</form>
<?php 
	page_footer();
?>