GFLAGS = --path gource.txt \
	 --user-image-dir avatars \
	 --hide progress \
	 --hide mouse \
	 --disable-auto-rotate \
	 --disable-input \
	 --elasticity 0.4 \
	 --user-friction 0.1 \
	 --background-colour 000000 \
	 --start-date '2024-05-24 07:00:00' \
	 -1920x1080 \
	 --stop-at-end \
	 --highlight-users \
	 --output-ppm-stream - \


video-fast: setup
	gource -c 4 $(GFLAGS) \
	       | ffmpeg -i - -vf "setpts=0.3*PTS" gource-fast.mp4

video-slow: setup
	gource -c 2 $(GFLAGS) \
	       | ffmpeg -i - gource-slow.mp4
setup:
	rm gource*
	gource --output-custom-log gource.txt
	(IFS="|"; while read wrong_name name; do sed -i "s/|$$wrong_name|/|$$name|/g" gource.txt; done < alias.txt)
