const plainText = Array.from("CRYPTOGRAPHIE");

const dico = Array.from("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
const chiffredText = [];
for (const letter of plainText) {
  chiffredText.push(
    dico[
      dico.findIndex((item) => item == letter) + 3 >= dico.length
        ? dico.findIndex((item) => item == letter) - dico.length
        : dico.findIndex((item) => item == letter) + 3
    ],
  );
}
console.log(chiffredText.join());
