<?php
	/**
	 * This file lists boards to user.
	 * $Id$
	 */
	require("funcs.php");
	if ($loginok !=1 )
		html_nologin();
	else
	{
		html_init("gb2312");
		if (isset($_GET["group"]))
			$group = $_GET["group"];
		else
			$group = 0;
		settype($group, "integer");
		if (isset($_GET["yank"]))
			$yank = $_GET["yank"];
		else
			$yank = 0;
		settype($yank, "integer");
		if (isset($_GET["group2"]))
			$group2 = $_GET["group2"];
		else
			$group2 = 0;
		settype($group, "integer");
		if ($group < 0 || $group > sizeof($section_nums))
			html_error_quit("����Ĳ���");
		$boards = bbs_getboards($section_nums[$group], $group2, $yank);
		//print_r($boards);
		if ($boards == FALSE)
			html_error_quit("��Ŀ¼��δ�а���");
?>
<style type="text/css">A {color: #0000f0}</style>
<body>
<center><?php echo BBS_FULL_NAME; ?> -- ���������� [<?php echo $section_names[$group][0]; ?>]
<?php
	if( $group2 != -2 ){
		if ($yank == 0)
		{
?>
[<a href="<?php echo $_SERVER["PHP_SELF"]; ?>?group=<?php echo $group; ?>&yank=1">�������п���</a>]
<?php
		}
		else
		{
?>
[<a href="<?php echo $_SERVER["PHP_SELF"]; ?>?group=<?php echo $group; ?>">�����Ѷ��Ŀ���</a>]
<?php
		}
	}
?>
[<a href="/bbssec.php">����������</a>]
<hr class="default"/>
<table width="610">
<tr>
<td>���</td>
<td>δ</td>
<td>����������</td>
<td>���</td>
<td>��������</td>
<td>����</td>
<td>������</td>
</tr>
<?php
		$brd_name = $boards["NAME"]; // Ӣ����
		$brd_desc = $boards["DESC"]; // ��������
		$brd_class = $boards["CLASS"]; // �������
		$brd_bm = $boards["BM"]; // ����
		$brd_artcnt = $boards["ARTCNT"]; // ������
		$brd_unread = $boards["UNREAD"]; // δ�����
		$brd_zapped = $boards["ZAPPED"]; // �Ƿ� z ��
		$brd_flag = $boards["FLAG"]; //flag
		$brd_bid = $boards["BID"]; //flag
		$rows = sizeof($brd_name);
		for ($i = 0; $i < $rows; $i++)	
		{
		if ($brd_flag[$i]&BBS_BOARD_GROUP)
		  $brd_link="/bbsboa.php?group=" . $group . "&group2=" . $brd_bid[$i];
		else
		  $brd_link="/bbsdoc.php?board=" . urlencode($brd_name[$i]);
?>
<tr>
<td><?php echo $i+1; ?></td>
<td>
<?php
			if ($brd_flag[$i]&BBS_BOARD_GROUP)
				echo "��";
			else
			if ($brd_unread[$i] == 1)
				echo "��";
			else
				echo "��";
?>
</td>
<td>
<?php
			if ($brd_zapped[$i] == 1)
				echo "*";
			else
				echo "&nbsp;";
?><a href="<?php echo $brd_link; ?>"><?php echo $brd_name[$i]; ?></a>
</td>
<td><?php echo $brd_class[$i]; ?></td>
<td>
<a href="<?php echo $brd_link; ?>"><?php echo $brd_desc[$i]; ?></a>
</td>
<td>
<?php
			$bms = explode(" ", trim($brd_bm[$i]));
			if (strlen($bms[0]) == 0 || $bms[0][0] <= chr(32))
				echo "����������";
			else
			{
				if (!ctype_print($bms[0][0]))
					echo $bms[0];
				else
				{
?>
<a href="/bbsqry.php?userid=<?php echo $bms[0]; ?>"><?php echo $bms[0]; ?></a>
<?php
				}
			}
?>
</td>
<td><?php 
if (!($brd_flag[$i]&BBS_BOARD_GROUP))
	echo $brd_artcnt[$i]; 
else echo "Ŀ¼";	
?></td>
</tr>
<?php
		} //		for ($i = 0; $i < $rows; $i++)
?>
</table>
<hr class="default"/>
</center>
<?php
		html_normal_quit();
	}
?>
