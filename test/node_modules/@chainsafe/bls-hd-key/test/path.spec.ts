import {pathToIndices} from "../src";
import {expect} from "chai";

describe("utils", function () {

  describe("validate path", function () {

    it("should not be valid", function () {
      expect(() => pathToIndices("a/12381/3600/0/0")).to.throw();
      expect(() => pathToIndices("m/12382/3600/0/0")).to.throw();
      expect(() => pathToIndices("m/12381/3600'/0/0")).to.throw();
      expect(() => pathToIndices("m/12381/3600'/0")).to.throw();
      expect(() => pathToIndices("m/12381/3600/0")).to.throw();
      expect(() => pathToIndices("m/12381/600/0/0", 60)).to.throw();
      expect(() => pathToIndices("m/12381/3600/1/12010124124124124124124124124")).to.throw();
    });

    it("should be valid", function () {
      expect(() => pathToIndices("m/12381/3600/0/0")).to.not.throw();
      expect(() => pathToIndices("m/12381/3600/0/0/0")).to.not.throw();
      expect(() => pathToIndices("m/12381/60/0/0", null)).to.not.throw();
      expect(() => pathToIndices("m/12381/600/0/0", 600)).to.not.throw();
    });

  });

  describe("path to indices", function () {

    it("should convert", function () {
      const indices = pathToIndices("m/12381/3600/0/100");
      expect(indices.length).to.be.equal(4);
      expect(indices[0]).equals(12381);
      expect(indices[1]).equals(3600);
      expect(indices[2]).equals(0);
      expect(indices[3]).equals(100);
    });

  });

});
