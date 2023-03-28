package edu.fsu.omp.api;

import edu.fsu.omp.data.ProductDTO;
import edu.fsu.omp.service.ProductSearchService;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.HttpStatus;
import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.*;

import java.util.List;

@Slf4j
@Controller
@RequestMapping(path="/search")
public class SearchController {
    @Autowired
    private ProductSearchService searchService;
    @GetMapping()
    @ResponseStatus(HttpStatus.OK)
    public @ResponseBody List<ProductDTO> getProduct(@RequestParam(required = true) String query) {
        return searchService.search(query);
    }
}
