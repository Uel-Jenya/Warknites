import os
from youtube_transcript_api import YouTubeTranscriptApi
import yt_dlp

# ==========================================
# Configuration
# ==========================================
PLAYLISTS = [
    "https://www.youtube.com/playlist?list=PLVw56me2zrXRxM2LWI806MH7scYdLty2i",
    "https://www.youtube.com/playlist?list=PLVw56me2zrXTFL847aqSLu8WjEwdk-CMU",
    "https://www.youtube.com/playlist?list=PLVw56me2zrXQijd-9QGGHlxHBRqNpD_RR"
]
OUTPUT_DIR = "youtube_transcripts"

def get_playlist_videos(playlist_url):
    ydl_opts = {
        'extract_flat': True,
        'quiet': True
    }
    with yt_dlp.YoutubeDL(ydl_opts) as ydl:
        info = ydl.extract_info(playlist_url, download=False)
        return info.get('entries', [])

def main():
    os.makedirs(OUTPUT_DIR, exist_ok=True)
    ytt_api = YouTubeTranscriptApi()

    for p_idx, playlist_url in enumerate(PLAYLISTS, 1):
        print(f"\\nProcessing Playlist {p_idx}...")
        playlist_folder = os.path.join(OUTPUT_DIR, f"playlist_{p_idx}")
        os.makedirs(playlist_folder, exist_ok=True)
        
        try:
            videos = get_playlist_videos(playlist_url)
            print(f"Found {len(videos)} videos in playlist {p_idx}.")
        except Exception as e:
            print(f"Failed to fetch playlist {p_idx}: {e}")
            continue
        
        for v_idx, video in enumerate(videos, 1):
            video_id = video.get('id')
            title = video.get('title', f'video_{v_idx}')
            
            # Clean title for filesystem compatibility
            safe_title = "".join(c for c in title if c.isalnum() or c in (' ', '_', '-')).strip()
            transcript_file = os.path.join(playlist_folder, f"{v_idx:03d}_{safe_title}.txt")
            
            if os.path.exists(transcript_file):
                print(f"Skipping '{safe_title}' (already downloaded)")
                continue
                
            print(f"Fetching transcript for: {safe_title}")
            try:
                # Prioritize English transcripts (including region variants)
                transcript_list = ytt_api.list(video_id)
                transcript = transcript_list.find_transcript(['en', 'en-US', 'en-GB', 'en-ZA'])
                transcript_data = transcript.fetch()
                
                # Combine text from all transcript chunks
                transcript_text = "\\n".join([item.text for item in transcript_data])
                
                with open(transcript_file, 'w', encoding='utf-8') as f:
                    f.write(transcript_text)
                print(f"  -> Saved successfully!")
                
            except Exception as e:
                print(f"  -> Failed to fetch transcript. Error: {e}")

if __name__ == "__main__":
    main()