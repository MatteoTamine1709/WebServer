import express from "express";

const app = express();
app.use(express.static("../public"));

app.get("/", (req, res) => {
  res.send("Hello World!");
});

app.listen(3001, () => {
  console.log("Server is running on port 3001");
});
