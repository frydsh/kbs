<?php
    require("funcs.php");
    if ($loginok != 1)
		html_nologin();
    else
    {
        html_init("gb2312");
        if ($currentuser["userid"]=="guest")
            html_error_quit("�Ҵҹ��Ͳ����趨����");

		if( isset( $_GET["start"] ) ){
			$start = $_GET["start"];
			settype($start, "integer");
		}else
			$start = 0;

		$total = bbs_countfriends($currentuser["userid"]);

		if( $total < 0 ){
			html_error_quit("ϵͳ����");
		}
?>
<center>
<?php echo BBS_FULL_NAME; ?> -- �������� [ʹ����: <?php echo $currentuser["userid"];?>]
<hr color="green"><br>
<?php
		if( $total != 0 ){
			if($start >= $total){
				$start = $total - 20;
				if($start < 0) $start = 0;
			}else if($start < 0){
				$start = $total - 20;
				if($start < 0) $start = 0;
			}

			$friends = bbs_getfriends($currentuser["userid"], $start);
			if ($friends == FALSE){
				html_error_quit("ϵͳ����1");
			}
?>
�����趨�� <?php echo $total;?> λ����<br>
<table border=1>
<tr><td>���</td><td>���Ѵ���</td><td>����˵��</td><td>ɾ������</td></tr>
<?php
			$i=0;
			
			foreach ($friends as $friend){
				$i++;
?>
<tr><td><?php echo $start+$i;?></td>
<td><a href="/bbsqry.php?userid=<?php echo $friend["ID"];?>"><?php echo $friend["ID"];?></a></td>
<td><?php echo $friend["EXP"];?></td>
<td>[<a onclick="return confirm('ȷʵɾ����?')" href="/bbsfdel.php?userid=<?php echo $friend["ID"];?>">ɾ��</a>]</td>
</tr>
<?php
			}
?>
</table>
<?php
		}
?>
<hr color="green">
[<a href="/bbsfadd.php">����µĺ���</a>]
<?php
		if( $start > 0 ){
?>
[<a href="/bbsfall.php?start=0">��һҳ</a>]
[<a href="/bbsfall.php?start=<?php if($start > 20) echo $start - 20; else echo "0";?>">��һҳ</a>]
<?php
		}
		if( $start < $total - 20 ){
?>
[<a href="/bbsfall.php?start=<?php echo $start + 20; ?>">��һҳ</a>]
[<a href="/bbsfall.php?start=-1">���һҳ</a>]
</center>
<?php
		}

		html_normal_quit();
    }
?>
