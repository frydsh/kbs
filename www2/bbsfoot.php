<?php
    $setboard=0;
    require("www2-funcs.php");
	login_init();
	page_header("状态", FALSE);
	
	if (strcmp($currentuser["userid"], "guest")) {
		$tn = bbs_mail_get_num($currentuser["userid"]);
		if ($tn) {
			$unread = $tn["newmail"];
			$total = $tn["total"];
		}
	}
?>
<script type="text/javascript">
<!--
	addBootFn(footerStart);
	var stayTime = <?php echo (time()-$currentuinfo["logintime"]); ?>;
	var serverTime = <?php echo (time() + intval(date("Z"))); ?>;
	var hasMail = <?php echo $unread ? "1" : "0"; ?>;
//-->
</script>
<body><div class="footer">时间[<span id="divTime"></span>] 在线[<?php echo bbs_getonlinenumber(); ?>]
帐号[<a href="bbsqry.php?userid=<?php echo $currentuser["userid"]; ?>" target="f3"><?php echo $currentuser["userid"]; ?></a>]
<?php
	if (isset($total)) {
echo "信箱[<a href=\"bbsmailbox.php?path=.DIR&title=收件箱\" target=\"f3\">";
		if ($unread) {
			echo $total . "封(有新信)</a>] <bgsound src='sound/newmail.mp3'>";
		} else {
			echo $total . "封</a>] ";
		}
	}
?>
停留[<span id="divStay"></span>]
</div></body>
</html>
