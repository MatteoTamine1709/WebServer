import * as dashjs from "dashjs";

const url = "/videos/quality/output.mpd";
const video = document.querySelector("#videoPlayer") as HTMLVideoElement;
const player = dashjs.MediaPlayer().create();
player.initialize(video, url);
video.addEventListener("loadedmetadata", () => {
  player.setQualityFor(
    "video",
    player.getBitrateInfoListFor("video")[-1].qualityIndex
  );
  console.log("loadedmetadata");
  console.log(player.getBitrateInfoListFor("video"));
  console.log(player.getBitrateInfoListFor("audio"));
  console.log(player.duration());
});

function setQuality(quality: number) {
  player.updateSettings({
    streaming: {
      abr: {
        autoSwitchBitrate: {
          video: false,
        },
      },
    },
  });
  player.setQualityFor("video", quality);
}

document.querySelector("#low")?.addEventListener("click", () => {
  setQuality(0);
});
document.querySelector("#medium")?.addEventListener("click", () => {
  setQuality(1);
});
document.querySelector("#high")?.addEventListener("click", () => {
  setQuality(2);
});
