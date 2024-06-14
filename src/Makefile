video-fast: setup
	gource --path gource.txt \
	       --user-image-dir avatars \
	       -c 4 \
	       ---start-date '2024-05-24 07:00:00' \
	       -1920x1080 \
	       --stop-at-end \
	       --highlight-users \
	       --output-ppm-stream - \
	       | ffmpeg -i - -vf "setpts=0.3*PTS" gource.mp4

video-slow: setup
	gource --path gource.txt \
	       --user-image-dir avatars \
	       -c 2 \
	       ---start-date '2024-05-24 07:00:00' \
	       -1920x1080 \
	       --stop-at-end \
	       --highlight-users \
	       --output-ppm-stream - \
	       | ffmpeg -i - gource.mp4
setup:
	rm gource.*
	gource --output-custom-log gource.txt
	(IFS="|"; while read wrong_name name; do sed -i "s/|$$wrong_name|/|$$name|/g" gource.txt; done < alias.txt)
